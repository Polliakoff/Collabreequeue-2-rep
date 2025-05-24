#!/usr/bin/env python3
import sys
from pathlib import Path
import json
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from shapely.geometry import shape, LineString

DATASET_DIR = Path("corridor_dataset")  # или передавайте через argv

def list_files():
    files = sorted(DATASET_DIR.glob("*/path_*.geojson"))
    if not files:
        print("❌ Не найдены файлы в", DATASET_DIR); sys.exit(1)
    for i,f in enumerate(files): print(f"{i:03d}: {f.relative_to(DATASET_DIR)}")
    return files

def plot_file(fpath):
    js = json.loads(fpath.read_text())
    polys = [shape(feat["geometry"]) for feat in js["features"] if feat["properties"]["kind"]=="safe"]
    line  = shape(next(feat["geometry"] for feat in js["features"] if feat["properties"]["kind"]=="track"))
    fig,ax = plt.subplots()
    for p in polys:
        ax.add_patch(mpatches.Polygon(list(p.exterior.coords),
                     facecolor="lightskyblue",alpha=.5,edgecolor="navy"))
    xs,ys = line.xy
    ax.plot(xs,ys,'--',lw=2,color='k')
    ax.set_aspect('equal')
    ax.set_title(fpath.relative_to(DATASET_DIR))
    plt.show()

def main():
    files = list_files()
    while True:
        idx = input("Введите номер (q — выход): ")
        if idx.strip().lower() in ("q","quit","exit"): break
        try:
            f = files[int(idx)]
            plot_file(f)
        except Exception as e:
            print("❌", e)

if __name__=="__main__":
    main()
