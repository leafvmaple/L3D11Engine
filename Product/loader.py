import ctypes

from py.animation import load_animation
from py.mesh import load_mesh, convert_mesh
from py.material import load_material

dll = ctypes.WinDLL("./LLoaderD.dll")

if __name__ == "__main__":
    load_material(dll, "data/source/player/F1/部件/F1_0000_head.JsonInspack")

    convert_mesh(dll, "data/source/player/F1/部件/F1_0000_head.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_body.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_hand.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_leg.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_belt.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/f1_new_face.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_hat.mesh")
    convert_mesh(dll, "data/source/player/F1/部件/F1_2206_hat.mesh")
    convert_mesh(dll, "data/source/item/weapon/brush/RH_brush_001.Mesh")
    load_animation(dll, "data/source/player/F1/动作/F1b02yd奔跑.ani")