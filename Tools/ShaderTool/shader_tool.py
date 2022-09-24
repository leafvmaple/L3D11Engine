import os
import subprocess
import sys

fxcpath = os.path.join(sys.path[0], "fxc.exe")

        # os.system('%s %s /dumpbin' % (fxcpath, outpath))

        # print(shotname, extension, len(data))

def Decompiler(path):
    for dirpath, dirnames, filenames in os.walk(path):
        for filename in filenames:
            shotname, extension = os.path.splitext(filename)
            if extension != ".fxo":
                continue

            f = open(os.path.join(dirpath, filename), 'rb')
            data = f.read()[0:-16]
            f.close()

            temppath = os.path.join(dirpath, shotname + ".temp")
            outpath = os.path.join(dirpath, shotname + ".asm")


            f = open(temppath, 'wb+')
            f.write(data)
            f.close()

            proc = subprocess.Popen([fxcpath, temppath, "/dumpbin"], stdout=subprocess.PIPE)
            outs, errs = proc.communicate()

            os.remove(temppath)

            f = open(outpath, 'wb+')
            f.write(outs)
            f.close()

def Compiler(path):
    print(fxcpath)
    for filename in os.listdir(path):
        shotname, extension = os.path.splitext(filename)
        if extension != ".fx":
            continue

        curpath = os.path.join(path, filename)
        tempath = os.path.join(path, shotname + '.tmp')
        outpath = os.path.join(path, "FXO", shotname + ".fxo")
        asmpath = os.path.join(path, shotname + '.asm')

        proc = subprocess.Popen([fxcpath, curpath, "/T", "vs_5_0", "/E", "VS", "/Fo", tempath, "/Fc", asmpath])
        outs, errs = proc.communicate()

        f = open(tempath, 'rb')
        data = f.read() + bytes(16)
        print(len(data))
        f.close()

Decompiler("F:\Github\L3D11Engine\Client\Res\Shader")