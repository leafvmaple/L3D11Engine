import ctypes
from ctypes import c_char, c_int, c_float, c_byte, c_ushort, c_ulong, POINTER

class Structure(ctypes.Structure):
    def to_dict(self):
        return struct_to_dict(self)

class XMFLOAT2(Structure):
    _pack_ = 8
    _fields_ = [("x", c_float), ("y", c_float)]

class XMFLOAT3(Structure):
    _pack_ = 8
    _fields_ = [("x", c_float), ("y", c_float), ("z", c_float)]

class XMFLOAT4(Structure):
    _pack_ = 8
    _fields_ = [("x", c_float), ("y", c_float), ("z", c_float), ("w", c_float)]

class XMFLOAT4X4(Structure):
    _pack_ = 8
    _fields_ = [
        ("_11", c_float), ("_12", c_float), ("_13", c_float), ("_14", c_float),
        ("_21", c_float), ("_22", c_float), ("_23", c_float), ("_24", c_float),
        ("_31", c_float), ("_32", c_float), ("_33", c_float), ("_34", c_float),
        ("_41", c_float), ("_42", c_float), ("_43", c_float), ("_44", c_float)
    ]

class CHAR30(ctypes.Array):
    _pack_ = 8
    _type_ = c_char
    _length_ = 30

    def to_dict(self):
        return self.value.decode('gbk')

def parse(obj):
    if isinstance(obj, Structure):
        return obj.to_dict()
    elif isinstance(obj, CHAR30):
        return obj.to_dict()
    elif isinstance(obj, ctypes.Array):
        return [parse(v) for v in obj]
    elif isinstance(obj, ctypes._Pointer):
        return None
    elif isinstance(obj, bytes):
        return obj.decode('gbk')
    return obj

def struct_to_dict(struct: Structure):
    result = {}
    for field_name, _ in struct._fields_:
        value = getattr(struct, field_name)
        result[field_name] = parse(value)
    return result

def pointer_to_array(pointer: ctypes._Pointer, *args):
    length, *param = args
    result = [pointer_to_array(pointer[i], *param) if len(param) > 0 else parse(pointer[i])
              for i in range(length)]

    return result
    

