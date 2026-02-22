# Basic OpenGL


## How to install "make" command and C/C++ compilers:


### For Windows (make):


#### Option 1 (Install with `Chocolatey`):

1. Install `Chocolatey` from following website: \
   https://chocolatey.org/install

2. After finishing the installation intall `make` using the following command on the CMD (Run as adimistrator):
   ```
   choco install make
   ```


#### Option 2 (Install with `MSYS2`):

1. Install `MSYS2` from the following website: \
   https://www.msys2.org/

2. Open `MSYS2` and intall `make` using the following command (on the opened console):
   ```
   pacman -S make
   ```


### For Windows (C/C++ compilers):

1. Go to the following website: \
   https://code.visualstudio.com/docs/cpp/config-mingw

2. Follow the guide on how to install the C/C++ compilers and validate that they are installed by using the following commands on the CMD:
   ```
   gcc --version
   g++ --version
   ```


### For MacOS (make `+` C/C++ compilers):

1. Install `Xcode Command Line Tools` using the following command on the Terminal:
   ```
   xcode-select --install
   ```

2. (Optional) Install `brew` from the following website: \
   https://brew.sh/

3. (Optional) Install with `brew` the `GLFW` library using the following command on the Terminal:
   ```
   brew install glfw
   ```


## For Linux (make `+` C/C++ compilers):

1. (Optional) Update the Linux OS using the following command on the Terminal:
   ```
   sudo apt update
   ```

2. Install `g++` using the following command on the Terminal:
   ```
   sudo apt install build-essential g++
   ```

3. Install `OpenGL` and `GLFW` using the following command on the Terminal:
   ```
   sudo apt install libgl-dev libglfw3-dev libxi-dev
   ```


## How to compile and run the OpenGL Engine using the Makefile:


### Using the CMD/Terminal:

1. Open the CMD/Terminal.

2. Navigate to the project folder using `cd` commands.

3. Run the following command:
   ```
   make
   ```

4. Run the following commands:
   ```
   cd bin
   ./main
   ```


### Using Visual Studio Code:

1. Download Visual Studio Code from: \
   https://code.visualstudio.com/download

2. Download the `C/C++ Extension Pack` from the Visual Studio Code Extensions or from: \
   https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack

3. Download the `Makefile Tools Extension` from the Visual Studio Code Extensions or from: \
   https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools

4. Configure the `Makefile Tools Extension` to compile the engine: using the available [Makefile](Makefile):

   1. Edit the `Makefile` option, and set `The path to the makefile of the project` as: `Makefile`.

   2. Click on the `...` button near the `Play` button, and then choose: `Makefile:Configure`.

   3. Edit the `Lunch target` option, and choose the first option: `main()`.

   4. Click on the `Play` button and validate that the program compiles and runs successfully.

`Notice:` With this tool you can run the OpenGL in Debugging mode as well.


## MacOS known issue with "libglfw.3.dylib" file:

The MacOS tends to block the file: "libglfw.3.dylib" which is crucial for running the OpenGL Engine. 
To remove the block, open the MacOS "Settings", go to "Privacy & Security", scroll down until you find the "libglfw.3.dylib" and choose to enable permission for it.


## Useful guides for building the OpenGL Engine:

- For Windows: \
  https://www.youtube.com/watch?v=hRInLNR9iRg

- For MacOS: \
  https://www.youtube.com/watch?v=7-dL6a5_B3I

- For Linux: \
  https://www.youtube.com/watch?v=JxDLGHil-Cw


## Useful YouTube guides:

- The Cherno: \
  https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2

- Victor Gordan: \
  https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-


## Library sources:

- GLFW: \
  https://www.glfw.org/

- GLAD: \
  https://glad.dav1d.de/

- GLEW (Not used, But works similar to GLAD): \
  https://glew.sourceforge.net/

- stb: \
  https://github.com/nothings/stb

- glm: \
  https://github.com/g-truc/glm/releases/tag/1.0.1


## PointNet (Assignment 3)

Python environment:
- Python 3.11 (venv recommended).

Setup (Windows PowerShell):
```
python -m venv PointNet/.venv
PointNet\.venv\Scripts\activate
pip install -r PointNet/requirements.txt
```

Troubleshooting (pip no-index):
- Current environment blocks index access: `PIP_NO_INDEX=1`.
- If installs fail with "No matching distribution found", unset it for the session:
```
$env:PIP_NO_INDEX=""
PointNet\.venv\Scripts\python -m pip install --index-url https://pypi.org/simple -r PointNet/requirements.txt
```

pip config (from `pip config list -v`):
```
For variant 'global', will try loading 'C:\ProgramData\pip\pip.ini'
For variant 'user', will try loading 'C:\Users\SwarkaBader\pip\pip.ini'
For variant 'user', will try loading 'C:\Users\SwarkaBader\AppData\Roaming\pip\pip.ini'
For variant 'site', will try loading 'C:\Users\SwarkaBader\Desktop\Graphics\Hw3\BasicOpenGL-main\PointNet\.venv\pip.ini'
:env:.disable-pip-version-check='1'
:env:.no-index='1'
```

pip errors (while trying to install packages):
```
ERROR: Could not find a version that satisfies the requirement wheel (from versions: none)
ERROR: No matching distribution found for wheel
ERROR: Could not find a version that satisfies the requirement numpy (from versions: none)
ERROR: No matching distribution found for numpy
```

pip errors (after clearing `PIP_NO_INDEX`, proxy refused connection):
```
WARNING: Retrying (Retry(total=4, connect=None, read=None, redirect=None, status=None)) after connection broken by 'ProxyError('Cannot connect to proxy.', NewConnectionError('<pip._vendor.urllib3.connection.HTTPSConnection object at 0x000001C34C55F290>: Failed to establish a new connection: [WinError 10061] No connection could be made because the target machine actively refused it'))': /simple/numpy/
WARNING: Retrying (Retry(total=3, connect=None, read=None, redirect=None, status=None)) after connection broken by 'ProxyError('Cannot connect to proxy.', NewConnectionError('<pip._vendor.urllib3.connection.HTTPSConnection object at 0x000001C34CA26B50>: Failed to establish a new connection: [WinError 10061] No connection could be made because the target machine actively refused it'))': /simple/numpy/
WARNING: Retrying (Retry(total=2, connect=None, read=None, redirect=None, status=None)) after connection broken by 'ProxyError('Cannot connect to proxy.', NewConnectionError('<pip._vendor.urllib3.connection.HTTPSConnection object at 0x000001C34CA27850>: Failed to establish a new connection: [WinError 10061] No connection could be made because the target machine actively refused it'))': /simple/numpy/
WARNING: Retrying (Retry(total=1, connect=None, read=None, redirect=None, status=None)) after connection broken by 'ProxyError('Cannot connect to proxy.', NewConnectionError('<pip._vendor.urllib3.connection.HTTPSConnection object at 0x000001C34CA27F90>: Failed to establish a new connection: [WinError 10061] No connection could be made because the target machine actively refused it'))': /simple/numpy/
WARNING: Retrying (Retry(total=0, connect=None, read=None, redirect=None, status=None)) after connection broken by 'ProxyError('Cannot connect to proxy.', NewConnectionError('<pip._vendor.urllib3.connection.HTTPSConnection object at 0x000001C34CA28990>: Failed to establish a new connection: [WinError 10061] No connection could be made because the target machine actively refused it'))': /simple/numpy/
ERROR: Could not find a version that satisfies the requirement numpy (from versions: none)
ERROR: No matching distribution found for numpy
```

Notebook + dataset (Kaggle):
- Notebook: https://www.kaggle.com/code/jeremy26/pointnet-shapenet-dataset
  - Save as `PointNet/pointnet-shapenet-dataset.ipynb`.
- Dataset: https://www.kaggle.com/datasets/jeremy26/shapenet-core-seg
  - Unzip under `PointNet/data/`.

Kaggle CLI examples (requires `kaggle` + API token):
```
kaggle kernels pull jeremy26/pointnet-shapenet-dataset -p PointNet
kaggle datasets download -d jeremy26/shapenet-core-seg -p PointNet/data
```
