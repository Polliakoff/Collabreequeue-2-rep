#!/usr/bin/env python3
"""
corridor_gui.py – центрированный широкий коридор c «штрафом» за
близость к льду. Поддерживает три режима: single, interactive, batch.
"""
import json, math, random, sys, requests, tkinter as tk
from pathlib import Path

import numpy as np, networkx as nx
from scipy import ndimage as ndi
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from shapely.geometry import shape, Polygon, LineString, mapping
from shapely.ops import unary_union
from matplotlib.widgets import RectangleSelector
from tkinter import ttk
from pathlib import Path

# ─── 0. Глобальные параметры ───────────────────────────────────
BBOX        = (83.05, 73.12, 83.12, 73.19)
ORIGINAL_BBOX = BBOX
LAYER       = "arctic_icechart_metno"
WIDTH_M     = 500
CLEAR_M0    = 80
GRID_M0     = 50
SIMPLIFY_M  = 20
MAX_RETRY   = 4
PENALTY_B   = 8.0
OUT_FILE    = Path("corridor.geojson")

AUTO_MARGIN_M = 5_000
MAX_AREA_M2   = 1.0e8
FRAG_M        = 800
GAP_FRAC      = 0.25
JITTER_FRAC   = 0.40
NEAR_CELLS    = 4

REMOVE_FRAC = 0.8
REMOVE_N    = None
RANDOM_SEED = 42

MIN_SPAN_KM = 30
MAX_SPAN_KM = 60
ROI_TRIES   = 120

INTERACTIVE_ROI = False
INTERACTIVE_AB  = False

NUM_SAMPLES = 200
TRAIN_FRAC  = 0.8
OUT_DIR     = Path("corridor_dataset")
BATCH_MODE  = False

# ─── 1. GUI для настройки перед работой ─────────────────────────
def launch_gui():
    root = tk.Tk(); root.title("corridor_gui")
    def _on_close():
        sys.exit(0)
    root.protocol("WM_DELETE_WINDOW", _on_close)
    entries = {}

    # расширяем список параметров
    params = [
      ("BBOX", ",".join(map(str, BBOX))),
      ("LAYER", LAYER),
      ("WIDTH_M", WIDTH_M),
      ("CLEAR_M0", CLEAR_M0),
      ("GRID_M0", GRID_M0),
      ("SIMPLIFY_M", SIMPLIFY_M),
      ("MAX_RETRY", MAX_RETRY),
      ("PENALTY_B", PENALTY_B),
      ("AUTO_MARGIN_M", AUTO_MARGIN_M),
      ("MAX_AREA_M2", MAX_AREA_M2),
      ("FRAG_M", FRAG_M),
      ("GAP_FRAC", GAP_FRAC),
      ("JITTER_FRAC", JITTER_FRAC),
      ("NEAR_CELLS", NEAR_CELLS),
      ("REMOVE_FRAC", REMOVE_FRAC if REMOVE_FRAC is not None else ""),
      ("REMOVE_N", REMOVE_N if REMOVE_N is not None else ""),
      ("RANDOM_SEED", RANDOM_SEED),
      ("MIN_SPAN_KM", MIN_SPAN_KM),
      ("MAX_SPAN_KM", MAX_SPAN_KM),
      ("ROI_TRIES", ROI_TRIES),
      ("NUM_SAMPLES", NUM_SAMPLES),
      ("TRAIN_FRAC", TRAIN_FRAC),
      ("OUT_FILE", str(OUT_FILE)),
      ("OUT_DIR", str(OUT_DIR)),
    ]

    # создаём строки ввода
    for i,(name,val) in enumerate(params):
        tk.Label(root, text=name).grid(row=i, column=0, sticky="w")
        e = tk.Entry(root)
        e.insert(0, str(val))
        e.grid(row=i, column=1)
        entries[name] = e

    # режим запуска
    mode_var = tk.StringVar(value="single")
    for j,(text,mode) in enumerate([
        ("Single","single"),
        ("Interactive","interactive"),
        ("Batch","batch"),
    ]):
        ttk.Radiobutton(
            root, text=text, variable=mode_var, value=mode
        ).grid(row=j, column=2, sticky="w")

    def on_run():
        g = globals()

        # Парсим BBOX из строки "lon1,lat1,lon2,lat2"
        bbox_str = entries["BBOX"].get()
        g["BBOX"] = tuple(map(float, bbox_str.split(",")))
        g["ORIGINAL_BBOX"] = g["BBOX"]

        # Строковые параметры
        g["LAYER"]    = entries["LAYER"].get()
        g["OUT_FILE"] = Path(entries["OUT_FILE"].get())
        g["OUT_DIR"]  = Path(entries["OUT_DIR"].get())

        # Числовые параметры
        g["WIDTH_M"]     = int(entries["WIDTH_M"].get())
        g["CLEAR_M0"]    = int(entries["CLEAR_M0"].get())
        g["GRID_M0"]     = int(entries["GRID_M0"].get())
        g["SIMPLIFY_M"]  = int(entries["SIMPLIFY_M"].get())
        g["MAX_RETRY"]   = int(entries["MAX_RETRY"].get())
        g["PENALTY_B"]   = float(entries["PENALTY_B"].get())
        g["AUTO_MARGIN_M"] = int(entries["AUTO_MARGIN_M"].get())
        g["MAX_AREA_M2"]   = float(entries["MAX_AREA_M2"].get())
        g["FRAG_M"]        = int(entries["FRAG_M"].get())
        g["GAP_FRAC"]      = float(entries["GAP_FRAC"].get())
        g["JITTER_FRAC"]   = float(entries["JITTER_FRAC"].get())
        g["NEAR_CELLS"]    = int(entries["NEAR_CELLS"].get())
        g["MIN_SPAN_KM"]   = float(entries["MIN_SPAN_KM"].get())
        g["MAX_SPAN_KM"]   = float(entries["MAX_SPAN_KM"].get())
        g["ROI_TRIES"]     = int(entries["ROI_TRIES"].get())
        g["NUM_SAMPLES"]   = int(entries["NUM_SAMPLES"].get())
        g["TRAIN_FRAC"]    = float(entries["TRAIN_FRAC"].get())
        g["RANDOM_SEED"]   = int(entries["RANDOM_SEED"].get())

        # REMOVE_N и REMOVE_FRAC — только один из них может быть задан
        rem_n = entries["REMOVE_N"].get().strip()
        rem_f = entries["REMOVE_FRAC"].get().strip()
        g["REMOVE_N"]    = int(rem_n)    if rem_n else None
        g["REMOVE_FRAC"]= float(rem_f)  if rem_f else None

        # Режим работы
        m = mode_var.get()
        g["INTERACTIVE_ROI"] = (m=="interactive")
        g["INTERACTIVE_AB"]  = (m=="interactive")
        g["BATCH_MODE"]      = (m=="batch")

        root.destroy()

    tk.Button(root, text="Run", command=on_run).grid(
        row=len(params), column=0, columnspan=3, pady=10
    )

    root.mainloop()

# ─── 2. Утилиты по данным и геометрии ───────────────────────────
def m2deg_lon(m, lat):
    return m / (111_320 * math.cos(math.radians(lat)))

def fetch_ice(bbox, layer):
    lo1,la1,lo2,la2=bbox
    url = (f"https://geos.polarview.aq/geoserver/wfs?"
           f"service=WFS&version=1.0.0&request=GetFeature"
           f"&typeName=polarview:{layer}"
           f"&bbox={lo1},{la1},{lo2},{la2},EPSG:4326"
           "&outputFormat=application/json&srsName=EPSG:4326")
    try:
        js = requests.get(url, timeout=15).json()
        if js.get("features"):
            print(f"✓ PolarView: {len(js['features'])} полигона льда")
            return js
    except: pass
    return None

def synthetic(bbox):
    lo1,la1,lo2,la2=bbox; dx,dy=lo2-lo1,la2-la1
    polys = [
        [(lo1+.015*dx,la1+.02*dy),(lo1+.035*dx,la1+.02*dy),
         (lo1+.035*dx,la1+.28*dy),(lo1+.015*dx,la1+.28*dy)],
        [(lo1+.30*dx,la1+.05*dy),(lo1+.34*dx,la1+.08*dy),
         (lo1+.32*dx,la1+.11*dy),(lo1+.28*dx,la1+.08*dy)],
        [(lo1+.55*dx,la1+.35*dy),(lo1+.80*dx,la1+.35*dy),
         (lo1+.80*dx,la1+.45*dy),(lo1+.55*dx,la1+.45*dy)],
        [(lo1+.50*dx,la1+.60*dy),(lo1+.60*dx,la1+.65*dy),
         (lo1+.55*dx,la1+.70*dy),(lo1+.45*dx,la1+.65*dy)],
        [(lo1+.17*dx,la1+.55*dy),(lo1+.25*dx,la1+.55*dy),
         (lo1+.30*dx,la1+.60*dy),(lo1+.22*dx,la1+.65*dy),
         (lo1+.14*dx,la1+.65*dy),(lo1+.09*dx,la1+.60*dy)],
        [(lo1+.67*dx,la1+.15*dy),(lo1+.75*dx,la1+.15*dy),
         (lo1+.75*dx,la1+.25*dy),(lo1+.67*dx,la1+.25*dy),
         (lo1+.67*dx,la1+.23*dy),(lo1+.73*dx,la1+.23*dy),
         (lo1+.73*dx,la1+.17*dy),(lo1+.67*dx,la1+.17*dy)],
    ]
    feats=[{"type":"Feature","properties":{},
            "geometry":{"type":"Polygon","coordinates":[p+[p[0]]]}
           } for p in polys]
    return {"type":"FeatureCollection","features":feats}

def area_m2(poly, lat):
    return poly.area * (111_320**2 * math.cos(math.radians(lat)))

from shapely.ops import split
def split_polygon_grid(poly, step_m, lat, max_lines=250,
                       gap_frac=GAP_FRAC, jitter_frac=JITTER_FRAC):
    step = m2deg_lon(step_m, lat)
    minx,miny,maxx,maxy=poly.bounds
    nx=min(int((maxx-minx)/step), max_lines)
    ny=min(int((maxy-miny)/step), max_lines)
    if nx*ny==0: return [poly]
    dx,dy=(maxx-minx)/nx,(maxy-miny)/ny
    grid=[]
    for i in range(1,nx):
        grid.append(LineString([(minx+i*dx,miny),(minx+i*dx,maxy)]))
    for j in range(1,ny):
        grid.append(LineString([(minx,miny+j*dy),(maxx,miny+j*dy)]))
    cuts=[poly]
    for g in grid:
        tmp=[]
        for p in cuts:
            try: tmp.extend(split(p,g).geoms)
            except: tmp.append(p)
        cuts=tmp
    out=[]
    gap, jit = step*gap_frac, step*jitter_frac
    for p in cuts:
        shr = p.buffer(-gap)
        if shr.is_empty: continue
        pieces = [shr] if shr.geom_type=="Polygon" else list(shr.geoms)
        for q in pieces:
            coords=[(x+random.uniform(-jit,jit),y+random.uniform(-jit,jit))
                    for x,y in list(q.exterior.coords)[:-1]]
            coords.append(coords[0])
            p2=Polygon(coords).buffer(0)
            if p2.is_valid and not p2.is_empty: out.append(p2)
    return out

# ─── 3. Интерактивные хелперы ───────────────────────────────────
def pick_roi_on_map(ax, base_bbox):
    coords={}
    def onsel(e, r):
        coords['bbox']=(min(e.xdata,r.xdata),min(e.ydata,r.ydata),
                        max(e.xdata,r.xdata),max(e.ydata,r.ydata))
    rs=RectangleSelector(ax, onsel, useblit=True, button=[1],
                         spancoords='data', minspanx=0, minspany=0,
                         interactive=True)
    plt.connect('key_press_event', lambda e: plt.close() if e.key=='enter' else None)
    plt.show()
    return coords.get('bbox')

def pick_two_points(ax):
    pts=plt.ginput(2, timeout=-1)
    return pts if len(pts)==2 else None

def nearest_water_cell(x,y,xs,ys,mask):
    ix0,iy0 = np.abs(xs-x).argmin(), np.abs(ys-y).argmin()
    if mask[iy0,ix0]: return ix0,iy0
    max_r=max(mask.shape)
    for r in range(1,max_r):
        for dx in range(-r,r+1):
            for dy in range(-r,r+1):
                ix,iy=ix0+dx,iy0+dy
                if 0<=ix<mask.shape[1] and 0<=iy<mask.shape[0] and mask[iy,ix]:
                    return ix,iy
    raise ValueError("No water nearby")

def random_roi(ice_polys, base_bbox, min_km, max_km, tries=ROI_TRIES):
    lo0,la0,lo1,la1=base_bbox
    lat_mid=(la0+la1)/2
    min_dx, max_dx = m2deg_lon(min_km*1e3,lat_mid), m2deg_lon(max_km*1e3,lat_mid)
    min_dy, max_dy = min_km*1e3/111320, max_km*1e3/111320
    ice_u=unary_union(ice_polys)
    for _ in range(tries):
        dx,dy= random.uniform(min_dx,max_dx), random.uniform(min_dy,max_dy)
        cx,cy= random.uniform(lo0+dx/2,lo1-dx/2), random.uniform(la0+dy/2,la1-dy/2)
        cand=(cx-dx/2,cy-dy/2,cx+dx/2,cy+dy/2)
        if ice_u.intersects(Polygon([(cand[0],cand[1]),(cand[2],cand[1]),
                                     (cand[2],cand[3]),(cand[0],cand[3])])):
            return cand
    print("⚠️ ROI failed, using base_bbox")
    return base_bbox

# ─── 4. Подготовка льдов (единый предрасчёт) ────────────────────
def prepare_ice():
    global BBOX, ORIGINAL_BBOX, ice_polys
    synthetic_js = synthetic(BBOX)
    ice_js = fetch_ice(BBOX, LAYER) or synthetic_js
    if ice_js is synthetic_js:
        print("⚠️ PolarView нет – синтетика")
    else:
        raw=[shape(f["geometry"]).buffer(0) for f in ice_js["features"]]
        ib=unary_union(raw).bounds
        if ib:
            latm=(ib[1]+ib[3])/2; m=m2deg_lon(AUTO_MARGIN_M,latm)
            BBOX=(ib[0]-m,ib[1]-m,ib[2]+m,ib[3]+m)
            print(f"✨ Cropped BBOX →{BBOX}")
    ORIGINAL_BBOX=BBOX
    # раскладка + дробление
    ice_polys=[]; latm=(BBOX[1]+BBOX[3])/2
    for f in (ice_js["features"]):
        g=shape(f["geometry"]).buffer(0)
        parts=[g] if g.geom_type=="Polygon" else list(g.geoms)
        for p in parts:
            if area_m2(p,latm)<=MAX_AREA_M2:
                ice_polys.append(p)
            else:
                ice_polys.extend(split_polygon_grid(p,FRAG_M,latm))
    # прореживание
    random.seed(RANDOM_SEED)
    if REMOVE_N:
        k=min(REMOVE_N,len(ice_polys)-1)
        ice_polys=random.sample(ice_polys,len(ice_polys)-k)
    elif 0<REMOVE_FRAC<1:
        k=int(len(ice_polys)*REMOVE_FRAC)
        ice_polys=random.sample(ice_polys,len(ice_polys)-k)
    random.seed()
    print(f"✨ ice_polys count → {len(ice_polys)}")

# ─── 5. Построение одного маршрута ──────────────────────────────
def generate_one(seed_idx):
    global BBOX
    # random.seed(RANDOM_SEED+seed_idx)
    BBOX = ORIGINAL_BBOX
    # ROI
    if INTERACTIVE_ROI:
        fig,ax=plt.subplots(); 
        for p in ice_polys:
            for sub in ([p] if p.geom_type=="Polygon" else p.geoms):
                ax.add_patch(mpatches.Polygon(list(sub.exterior.coords),
                                              facecolor="orange",edgecolor="k"))
        ax.set_xlim(BBOX[0], BBOX[2])
        ax.set_ylim(BBOX[1], BBOX[3])
        ax.set_aspect('equal'); ax.set_title("Draw ROI, Enter→")
        cand=pick_roi_on_map(ax,BBOX)
        if cand: BBOX=cand
        print(f"ROI→{BBOX}")
    else:
        # random.seed(RANDOM_SEED+seed_idx)
        BBOX=random_roi(ice_polys,BBOX,MIN_SPAN_KM,MAX_SPAN_KM)
    # вычисления
    lat_c=(BBOX[1]+BBOX[3])/2
    W_HALF=m2deg_lon(WIDTH_M/2,lat_c)
    simpl_tol=m2deg_lon(SIMPLIFY_M,lat_c)
    REG=Polygon([(BBOX[0],BBOX[1]),(BBOX[2],BBOX[1]),
                 (BBOX[2],BBOX[3]),(BBOX[0],BBOX[3])])
    safe = None
    for attempt in range(MAX_RETRY):
        clr_m  = CLEAR_M0 * (0.6 ** attempt)
        grid_m = GRID_M0  * (0.8 ** attempt)
        CLEAR  = m2deg_lon(clr_m, lat_c)
        GRID   = m2deg_lon(grid_m, lat_c)

        ICE   = unary_union(ice_polys).buffer(CLEAR) if ice_polys else None
        WATER = REG.difference(ICE)
        if WATER.is_empty:
            continue

        # растровая сетка и маска воды
        xs = np.arange(BBOX[0], BBOX[2] + GRID, GRID)
        ys = np.arange(BBOX[1], BBOX[3] + GRID, GRID)
        nx_, ny_ = len(xs), len(ys)
        mask = np.zeros((ny_, nx_), bool)
        for j, y in enumerate(ys):
            segs = LineString([(BBOX[0], y), (BBOX[2], y)]).intersection(WATER)
            for s in getattr(segs, "geoms", [segs]):
                if not s.is_empty:
                    mask[j, (xs >= s.bounds[0]) & (xs <= s.bounds[2])] = True
        if mask.sum() < 2:
            continue

        # distance transform
        dist = ndi.distance_transform_edt(mask)

        # строим 8-связный граф
        G = nx.grid_2d_graph(nx_, ny_)
        for i in range(nx_ - 1):
            for j in range(ny_ - 1):
                G.add_edge((i, j), (i + 1, j + 1))
                G.add_edge((i + 1, j), (i, j + 1))
        # удаляем лед
        G.remove_nodes_from([(i, j) for j in range(ny_) for i in range(nx_) if not mask[j, i]])

        # взвешиваем
        for u, v in G.edges():
            di, dj = abs(u[0] - v[0]), abs(u[1] - v[1])
            base = math.hypot(di, dj)
            d    = (dist[u[1], u[0]] + dist[v[1], v[0]]) / 2
            G.edges[u, v]["weight"] = base * (1 + PENALTY_B / (d + 1))

        # выбираем A/B
        water_idx   = np.column_stack(mask.nonzero())
        close_nodes = water_idx[dist[water_idx[:,0], water_idx[:,1]] < NEAR_CELLS]
        if len(close_nodes) >= 2:
            A_idx = close_nodes[random.randrange(len(close_nodes))]
            d2    = ((close_nodes - A_idx) ** 2).sum(axis=1)
            B_idx = close_nodes[d2.argmax()]
        else:
            A_idx = water_idx[random.randrange(len(water_idx))]
            d2    = ((water_idx - A_idx) ** 2).sum(axis=1)
            B_idx = water_idx[d2.argmax()]

        # интерактивный A/B
        if INTERACTIVE_AB:
            fig_ab, ax_ab = plt.subplots()
            for p in ice_polys:
                for sub in ([p] if p.geom_type=="Polygon" else p.geoms):
                    ax_ab.add_patch(mpatches.Polygon(
                        list(sub.exterior.coords), facecolor="orange", edgecolor="k"))
            ax_ab.set_xlim(BBOX[0], BBOX[2]); ax_ab.set_ylim(BBOX[1], BBOX[3])
            ax_ab.set_aspect('equal'); ax_ab.set_title("Click START then END, Enter")
            pts = pick_two_points(ax_ab)
            if pts:
                (xA,yA),(xB,yB) = pts
                sx,sy = nearest_water_cell(xA,yA,xs,ys,mask)
                gx,gy = nearest_water_cell(xB,yB,xs,ys,mask)
                A_idx, B_idx = np.array([sy,sx]), np.array([gy,gx])
                print("✨ START/END вручную привязаны к воде")

        sx, sy = int(A_idx[1]), int(A_idx[0])
        gx, gy = int(B_idx[1]), int(B_idx[0])

        try:
            path = nx.astar_path(G, (sx,sy), (gx,gy),
                                 heuristic=lambda u,v: math.hypot(u[0]-v[0],u[1]-v[1]),
                                 weight='weight')
            print(f"✓ Путь найден (попытка {attempt+1})")
            break
        except nx.NetworkXNoPath:
            print(f"✗ Нет пути при clear={clr_m:.0f} grid={grid_m:.0f}")
    else:
        sys.exit("❌ Не удалось построить путь ни при одном CLEAR/GRID")

    # линия и упрощение
    line = LineString([(xs[i], ys[j]) for i,j in path]).simplify(GRID * 0.5)

    # формируем «безопасный» полигон
    buf   = line.buffer(W_HALF, cap_style=2, join_style=2)
    safe0 = WATER.intersection(buf)
    parts = list(safe0.geoms) if safe0.geom_type=="MultiPolygon" else [safe0]
    cands = [p for p in parts if p.covers(line)]
    if not cands:
        cands = sorted(parts, key=lambda p: p.intersection(line).length, reverse=True)
    safe = Polygon(cands[0].exterior.coords).simplify(simpl_tol, preserve_topology=True)

    return safe, line

# ─── 6. Batch / Single / Interactive main ─────────────────────
def single_main():
    safe, line = generate_one(0)
    if not BATCH_MODE:
        # 1) сохранить GeoJSON
        geo = {
            "type": "FeatureCollection",
            "features": [
                {"type":"Feature","properties":{"ver":"wide v4"},
                 "geometry": mapping(safe)},
                {"type":"Feature","properties":{"ver":"path"},
                 "geometry": mapping(line)}
            ]
        }
        OUT_FILE.write_text(json.dumps(geo, indent=2))
        print("✓ GeoJSON →", OUT_FILE)

        # 2) отрисовать лед, ROI, safe-коридор и трек
        fig, ax = plt.subplots()
        # айс‐полигоны
        for p in ice_polys:
            subs = [p] if p.geom_type=="Polygon" else p.geoms
            for sub in subs:
                ax.add_patch(mpatches.Polygon(
                    list(sub.exterior.coords),
                    facecolor="orange", edgecolor="k"
                ))
        # контур ROI
        ax.add_patch(mpatches.Rectangle(
            (BBOX[0], BBOX[1]),
            BBOX[2] - BBOX[0],
            BBOX[3] - BBOX[1],
            fill=False, edgecolor="red", linewidth=2
        ))
        # safe-коридор
        ax.add_patch(mpatches.Polygon(
            list(safe.exterior.coords),
            facecolor="lightskyblue", alpha=.35, edgecolor="navy"
        ))
        # центральная линия
        ax.plot(*line.xy, '--', lw=2)
        # точки старта/финиша
        x0, y0 = line.coords[0]
        x1, y1 = line.coords[-1]
        ax.scatter(x0, y0, color='k')
        ax.scatter(x1, y1, color='k')

        ax.set_aspect('equal')
        ax.set_title(f"Широкий коридор ({WIDTH_M} м)")
        plt.show()

def batch_main():
    global INTERACTIVE_ROI, INTERACTIVE_AB
    n_train=int(NUM_SAMPLES*TRAIN_FRAC)
    (OUT_DIR/"train").mkdir(exist_ok=True,parents=True)
    (OUT_DIR/"test").mkdir(exist_ok=True,parents=True)
    ok=0
    for k in range(NUM_SAMPLES):
        INTERACTIVE_ROI=INTERACTIVE_AB=False; plt.ioff()
        res=generate_one(k)
        if not res: continue
        safe,line=res
        subset="train" if ok<n_train else "test"
        fout=OUT_DIR/subset/f"path_{ok:04d}.geojson"
        geo={"type":"FeatureCollection","features":[
             {"type":"Feature","properties":{"kind":"safe"},
              "geometry":mapping(safe)},
             {"type":"Feature","properties":{"kind":"track"},
              "geometry":mapping(line)}
        ]}
        fout.write_text(json.dumps(geo))
        print(f"Saved → {fout}")
        ok+=1
    print(f"Done: {ok} files (train {n_train}, test {ok-n_train})")

# ─── 7. Точка входа ────────────────────────────────────────────
def main():
    launch_gui()
    prepare_ice()
    if BATCH_MODE: batch_main()
    else:          single_main()

if __name__=="__main__":
    main()
