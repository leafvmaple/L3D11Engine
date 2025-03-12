from .base import Structure, XMFLOAT2, XMFLOAT3, XMFLOAT4, XMFLOAT4X4, CHAR30, pointer_to_array, save_json
from .base import rotate, scale_rotate
import ctypes
from ctypes import c_char_p, c_int, c_float, c_byte, c_ushort, c_ulong, POINTER
import os

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

    def to_dict(self):
        data = super().to_dict()
        data['pChildNames'] = pointer_to_array(self.pChildNames, self.nChildCount)

        return data

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
        data['pSubsetVertexCount'] = pointer_to_array(self.pSubsetVertexCount, self.nSubsetCount)

        return data


def load_mesh(dll, file_path):
    dll.LoadMesh.argtypes = [POINTER(MESH_DESC), POINTER(MESH_SOURCE)]
    dll.LoadMesh.restype = None

    desc = MESH_DESC(szName=file_path.encode('gbk'))
    source = MESH_SOURCE()

    dll.LoadMesh(ctypes.byref(desc), ctypes.byref(source))
    data = source.to_dict()

    save_json(data, file_path)

    return data

def convert_vertex(vertex):
    position = scale_rotate([vertex["Position"]["x"], vertex["Position"]["y"], vertex["Position"]["z"]])
    normal = rotate([vertex["Normal"]["x"], vertex["Normal"]["y"], vertex["Normal"]["z"]])
    tangent = rotate([vertex["Tangent"]["x"], vertex["Tangent"]["y"], vertex["Tangent"]["z"]])
    return {
            "px" : position[0],
            "py" : position[1],
            "pz" : position[2],
            "nx" : normal[0],
            "ny" : normal[1],
            "nz" : normal[2],
            "tx" : tangent[0],
            "ty" : tangent[1],
            "tz" : tangent[2],
            "u": vertex["TexCoords"]["x"],
            "v": vertex["TexCoords"]["y"]
        }

def convert_index(index):
    return index

def convert_mesh(dll, file_path):
    data = load_mesh(dll, file_path)
    vertex_res = {
        "vertex_buffer": [],
        "index_buffer": data["pIndices"],
        "bind": [],
    }
    for vertex in data["pVertices"]:
        vertex_res["vertex_buffer"].append(convert_vertex(vertex))
        bind = {}
        cnt = 0
        for i in range(4):
            if vertex["BoneIndices"][i] >= 0:
                bind[f"index{cnt}"] = vertex["BoneIndices"][i]
                bind[f"weight{cnt}"] = vertex["BoneWeights"][i]
                cnt += 1
        vertex_res["bind"].append(bind)

    name, _ = os.path.splitext(file_path)
    save_json(vertex_res, f"{name}.mesh_bind")

    bone_res = {
        "bones_map": []
    }
    for i in range(len(data["pBones"])):
        bone = data["pBones"][i]
        tpose_matrix = {}
        for j in range(4):
            for k in range(3):
                tpose_matrix[f"v{j * 3 + k }"] = bone["mInvPxPose"][f"_{j+1}{k+1}"]
        binding_pose = {
            "position": {
                "x": bone["mOffset"]["_14"],
                "y": bone["mOffset"]["_24"],
                "z": bone["mOffset"]["_34"],
            },
        }

        bone_res["bones_map"].append({
            "name": bone["szName"],
            "index": i,
            "binding_pose": binding_pose,
            "tpose_matrix": tpose_matrix
        })

    save_json(bone_res, f"{name}.skeleton")

    