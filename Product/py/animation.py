import ctypes
from ctypes import wintypes, POINTER, c_char, c_int, c_float, c_char_p, c_void_p
from .base import Structure, CHAR30, XMFLOAT3, XMFLOAT4, pointer_to_array, save_json
import json
import os

ANI_STRING_SIZE = 30
ANI_FILE_MASK = 0x414E494D
ANI_FILE_MASK_VERVION2 = 0x324E494D

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
        ("pBoneNames", POINTER(CHAR30)),
        ("pBoneRTS", POINTER(POINTER(RTS))),
        ("pFlag", POINTER(c_int))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pBoneNames'] = pointer_to_array(self.pBoneNames, self.nBonesCount)
        data['pBoneRTS'] = pointer_to_array(self.pBoneRTS, self.nBonesCount, self.nFrameCount)
        if self.pFlag:
            data['pFlag'] = pointer_to_array(self.pFlag, self.nBonesCount)

        return data

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


def load_animation(dll, file_path):
    LoadAnimation = dll.LoadAnimation
    LoadAnimation.argtypes = [POINTER(ANIMATION_DESC), POINTER(ANIMATION_SOURCE)]
    LoadAnimation.restype = None

    desc = ANIMATION_DESC(szFileName=file_path.encode('gbk'))
    source = ANIMATION_SOURCE()

    LoadAnimation(ctypes.byref(desc), ctypes.byref(source))
    data = source.to_dict()
    
    save_json(data, file_path)

    return data
