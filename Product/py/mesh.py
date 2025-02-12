from .base import Structure, XMFLOAT2, XMFLOAT3, XMFLOAT4, XMFLOAT4X4, CHAR30, pointer_to_array
import ctypes
from ctypes import c_char_p, c_int, c_float, c_byte, c_ushort, c_ulong, POINTER
import json

class BOUND_BOX(Structure):
    _pack_ = 8
    _fields_ = [
        ("fMinX", c_float), ("fMinY", c_float), ("fMinZ", c_float),
        ("fMaxX", c_float), ("fMaxY", c_float), ("fMaxZ", c_float)
    ]

class MESH_DESC(Structure):
    _pack_ = 8
    _fields_ = [("szName", c_char_p)]

class BONE_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("szName", CHAR30),
        ("nChildCount", c_int),
        ("pChildNames", POINTER(CHAR30)),
        ("mOffset", XMFLOAT4X4),
        ("mOffset2Parent", XMFLOAT4X4),
        ("mInvPxPose", XMFLOAT4X4),
        ("BoundingBox", BOUND_BOX)
    ]

class SOCKET_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("szName", CHAR30),
        ("szParentName", CHAR30),
        ("mOffset", XMFLOAT4X4)
    ]

class VERTEX_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("Position", XMFLOAT3),
        ("Normal", XMFLOAT3),
        ("Color", c_ulong),
        ("TexCoords", XMFLOAT2),
        ("BoneWeights", c_float * 4),
        ("BoneIndices", c_byte * 4),
        ("Tangent", XMFLOAT4)
    ]

class MESH_SOURCE(Structure):
    _pack_ = 8
    _fields_ = [
        ("nVertexFVF", c_int),
        ("nVertexSize", c_int),
        ("nFacesCount", c_int),
        ("nVerticesCount", c_int),
        ("pVertices", POINTER(VERTEX_SOURCE)),
        ("nIndexCount", c_int),
        ("pIndices", POINTER(c_ulong)),
        ("nBonesCount", c_int),
        ("pBones", POINTER(BONE_SOURCE)),
        ("nSocketCount", c_int),
        ("pSockets", POINTER(SOCKET_SOURCE)),
        ("nSubsetCount", c_int),
        ("pSubsetVertexCount", POINTER(c_ushort))
    ]

    def to_dict(self):
        data = super().to_dict()
        data['pVertices'] = pointer_to_array(self.pVertices, self.nVerticesCount)
        data['pIndices'] = pointer_to_array(self.pIndices, self.nIndexCount)
        data['pBones'] = pointer_to_array(self.pBones, self.nBonesCount)
        data['pSockets'] = pointer_to_array(self.pSockets, self.nSocketCount)
        data['pSubsetVertexCount'] = pointer_to_array(self.pSockets, self.nSubsetCount)

        return data


def load_mesh(dll, file_path):
    dll.LoadMesh.argtypes = [POINTER(MESH_DESC), POINTER(MESH_SOURCE)]
    dll.LoadMesh.restype = None

    desc = MESH_DESC(szName=file_path.encode('gbk'))
    source = MESH_SOURCE()

    dll.LoadMesh(ctypes.byref(desc), ctypes.byref(source))

    data = source.to_dict()
    with open('mesh.json', 'w') as file:
        json.dump(data, file, indent=4)

    return data