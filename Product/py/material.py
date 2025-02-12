import ctypes
from ctypes import wintypes, POINTER, c_char_p, c_char, c_uint, c_float
from .base import Structure, pointer_to_array, save_json

# Define constants
MAX_PATH = 260

# Define the structures
class MODEL_MATERIAL_DESC(Structure):
    _pack_ = 8
    _fields_ = [("szFileName", c_char_p)]

class MATERIAL_DEFINE_PARAM(Structure):
    _pack_ = 8
    _fields_ = [
        ("szName", c_char * MAX_PATH),
        ("szRegister", c_char * MAX_PATH),
        ("szValue", c_char * MAX_PATH),
        ("fValue", c_float)
    ]

class MATERIAL_DEFINE(Structure):
    _pack_ = 8
    _fields_ = [
        ("szName", c_char * MAX_PATH),
        ("szShaderName", c_char * MAX_PATH),
        ("nParam", c_uint),
        ("pParam", POINTER(MATERIAL_DEFINE_PARAM))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pParam'] = pointer_to_array(self.pParam, self.nParam)

        return data

class MATERIAL_SOURCE_PARAM(Structure):
    _pack_ = 8
    _fields_ = [
        ("szName", c_char * MAX_PATH),
        ("szType", c_char * MAX_PATH),
        ("szValue", c_char * MAX_PATH)
    ]

class MATERIAL_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nBlendMode", c_uint),
        ("nAlphaRef", c_uint),
        ("nAlphaRef2", c_uint),
        ("nTwoSideFlag", c_uint),
        ("nTexture", c_uint),
        ("pTexture", POINTER(MATERIAL_SOURCE_PARAM)),
        ("Define", MATERIAL_DEFINE)
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pTexture'] = pointer_to_array(self.pTexture, self.nTexture)

        return data

class MATERIAL_GROUP_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nSubset", c_uint),
        ("pSubset", POINTER(MATERIAL_SOURCE))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pSubset'] = pointer_to_array(self.pSubset, self.nSubset)
        
        return data

class MATERIAL_LOD_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nGroup", c_uint),
        ("pGroup", POINTER(MATERIAL_GROUP_SOURCE))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pGroup'] = pointer_to_array(self.pGroup, self.nGroup)
        
        return data

class MODEL_MATERIAL_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nLOD", c_uint),
        ("pLOD", POINTER(MATERIAL_LOD_SOURCE))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pLOD'] = pointer_to_array(self.pLOD, self.nLOD)

        return data

def load_material(dll, file_path):
    dll.LoadModelMaterial.argtypes = [POINTER(MODEL_MATERIAL_DESC), POINTER(MODEL_MATERIAL_SOURCE)]
    dll.LoadModelMaterial.restype = None

    desc = MODEL_MATERIAL_DESC(szFileName=file_path.encode('gbk'))
    source = MODEL_MATERIAL_SOURCE()

    dll.LoadModelMaterial(ctypes.byref(desc), ctypes.byref(source))
    data = source.to_dict()

    save_json(data, file_path)

    return data