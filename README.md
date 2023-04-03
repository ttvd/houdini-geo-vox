# houdini-geo-vox

[Houdini](http://www.sidefx.com/index.php) GEO node which lets you load [MagicaVoxel](https://ephtracy.github.io/) Voxel Vox files.

![houdini-geo-vox screenshot](http://i.imgur.com/stmKung.png)

## Binaries, Houdini 19.5
* [Windows, Houdini 19.5.493](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.7/houdini.geo.vox.19.5.493.rar)

## Binaries, Houdini 19.0
* [Windows, Houdini 19.0.561](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.6/houdini.geo.vox.19.0.561.rar)

## Binaries, Houdini 18.0
* [Windows, Houdini 18.0.348](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.5/houdini.geo.vox.18.0.348.updated.rar)

## Binaries, Houdini 17.5
* [Windows, Houdini 17.5.360](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.4/houdini.geo.vox.17.5.360.win.rar)

## Binaries, Houdini 16.0
* [Windows, Houdini 16.0.557](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.3/houdini.geo.vox.16.0.557.win.rar)

## Binaries, Houdini 15.0
* [Mac OS X, Houdini 15.0.434](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.2/houdini.geo.vox.15.0.434.osx.tar.gz)
* [Windows, Houdini 15.0.440](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.2/houdini.geo.vox.15.0.440.win.rar)

## Building

* Tested on Windows and Houdini 19.5.493
    * Generate a Visual Studio solution using premake:
        * premake5 --houdini-version=19.5.493 vs2022
    * Build the GEO Houdini dso GEO_Vox.dll.
    * Place the dso in the appropriate Houdini dso folder.
        * On Windows this would be C:\Users\your_username\Documents\houdini19.5\dso
        * Or your HOUDINI_DSO_PATH

## Usage

* Place a file SOP, select a .vox file.
* This will create a Houdini volume, containing LUT color values data.
* Use [LUT loading SOP](https://github.com/ttvd/houdini-sop-color-lut) to further process the data.
* Also have a look at [Volume center points SOP](https://github.com/ttvd/houdini-sop-volume-center-points) .
* Please see the [example HIP file.](example/)

## License

* Copyright Mykola Konyk, 2016
* Distributed under the [MS-RL License.](http://opensource.org/licenses/MS-RL)
* **To further explain the license:**
  * **You cannot re-license any files in this project.**
  * **That is, they must remain under the [MS-RL license.](http://opensource.org/licenses/MS-RL)**
  * **Any other files you add to this project can be under any license you want.**
  * **You cannot use any of this code in a GPL project.**
  * Otherwise you are free to do pretty much anything you want with this code.
