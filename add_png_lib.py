Import("env")
import subprocess
result = subprocess.run(["pkg-config", "--libs", "libpng"], capture_output=True, text=True)
if result.returncode == 0:
    libs = result.stdout.strip().split()
    for lib in libs:
        if lib.startswith("-l"):
            env.Append(LIBS=[lib[2:]])
        elif lib.startswith("-L"):
            env.Append(LIBPATH=[lib[2:]])
