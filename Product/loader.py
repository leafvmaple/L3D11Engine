import ctypes

from py.animation import load_animation
from py.mesh import load_mesh

dll = ctypes.WinDLL("./LLoaderD.dll")

if __name__ == "__main__":
    mesh = load_mesh(dll, "data/source/player/F1/部件/F1_0000_head.mesh")
    data = load_animation(dll, "data/source/player/F1/动作/F1b02yd奔跑.ani")