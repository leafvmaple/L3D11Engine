import ctypes
from ctypes import wintypes, POINTER, Structure, c_char, c_int, c_float, c_char_p, c_void_p
import json
import os
print("Current working directory:", os.getcwd())


ANI_STRING_SIZE = 30
ANI_FILE_MASK = 0x414E494D
ANI_FILE_MASK_VERVION2 = 0x324E494D

def structure_to_dict(struct):
    if not isinstance(struct, ctypes.Structure):
        raise TypeError("Expected a ctypes.Structure")

    result = {}
    for field_name, field_type in struct._fields_:
        value = getattr(struct, field_name)
        if isinstance(value, ctypes.Structure):
            result[field_name] = structure_to_dict(value)
        elif isinstance(value, (int, float)):
            result[field_name] = value
    return result

class ANIMATION_TYPE:
    ANIMATION_NONE = 0
    ANIMATION_VERTICES = 1
    ANIMATION_VERTICES_RELATIVE = 2
    ANIMATION_BONE = 10
    ANIMATION_BONE_RELATIVE = 11
    ANIMATION_BONE_16 = 12
    ANIMATION_BONE_RTS = 13
    ANIMATION_BLENDSHAPE = 14
    ANIMATION_VCIK = 15
    ANIMATION_COUNT = 16
    ANIMATION_FORCE_DWORD = 0xffffffff

class BONE_FLAG:
    BONE_FLAG_NONE = 0
    BONE_FLAG_AFFLINE = 1 << 0
    BONE_FLAG_NO_UPDATE = 1 << 1

class ANIMATION_DESC(Structure):
    _pack_ = 8
    _fields_ = [("szFileName", c_char_p)]


class XMFLOAT3(Structure):
    _pack_ = 8
    _fields_ = [("x", c_float), ("y", c_float), ("z", c_float)]
    
    def __str__(self):
        return str(self._fields_)

class XMFLOAT4(Structure):
    _pack_ = 8
    _fields_ = [("x", c_float), ("y", c_float), ("z", c_float), ("w", c_float)]
        
    def __str__(self):
        return str(vars(self))

class RTS(Structure):
    _pack_ = 8
    _fields_ = [
        ("Translation", XMFLOAT3),
        ("Scale", XMFLOAT3),
        ("Rotation", XMFLOAT4),
        ("Sign", c_float),
        ("SRotation", XMFLOAT4)
    ]

class ANIMATION_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nAnimationType", c_int),
        ("nBonesCount", c_int),
        ("nFrameCount", c_int),
        ("fFrameLength", c_float),
        ("nAnimationLength", c_int),
        ("pBoneNames", POINTER(c_char * ANI_STRING_SIZE)),
        ("pBoneRTS", POINTER(POINTER(RTS))),
        ("pFlag", POINTER(c_int))
    ]

    def __del__(self):
        if self.pBoneNames:
            ctypes.cast(self.pBoneNames, ctypes.POINTER(ctypes.c_char * ANI_STRING_SIZE * self.nBonesCount)).contents
        if self.pFlag:
            ctypes.cast(self.pFlag, ctypes.POINTER(ctypes.c_int * self.nBonesCount)).contents
        if self.pBoneRTS:
            for i in range(self.nBonesCount):
                if self.pBoneRTS[i]:
                    ctypes.cast(self.pBoneRTS[i], ctypes.POINTER(RTS * self.nFrameCount)).contents
            ctypes.cast(self.pBoneRTS, ctypes.POINTER(ctypes.POINTER(RTS) * self.nBonesCount)).contents

dll = ctypes.WinDLL("./LLoaderD.dll")

LoadAnimation = dll.LoadAnimation
LoadAnimation.argtypes = [POINTER(ANIMATION_DESC), POINTER(ANIMATION_SOURCE)]
LoadAnimation.restype = None

def load_animation(file_path):
    desc = ANIMATION_DESC(szFileName=file_path.encode('gbk'))
    source = ANIMATION_SOURCE()

    LoadAnimation(ctypes.byref(desc), ctypes.byref(source))
    return source

if __name__ == "__main__":
    file_path = "data/source/player/F1/动作/F1b02yd奔跑.ani"
    source = load_animation(file_path)
    data = structure_to_dict(source)
    data['pBoneName'] = [source.pBoneNames[i].value.decode('gbk') for i in range(source.nBonesCount)]
    data['pBoneRTS'] = [[
            structure_to_dict(source.pBoneRTS[i][j])
            for j in range(source.nFrameCount)
        ]
        for i in range(source.nBonesCount)
    ]
    if source.pFlag:
        data['pFlag'] = [source.pFlag[i] for i in range(source.nBonesCount)]

    with open('animation.json', 'w') as file:
        json.dump(data, file, indent=4)