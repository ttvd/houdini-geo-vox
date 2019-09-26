# houdini-geo-vox

[Houdini](http://www.sidefx.com/index.php) GEO node which lets you load [MagicaVoxel](https://ephtracy.github.io/) Voxel Vox files.

![houdini-geo-vox screenshot](http://i.imgur.com/stmKung.png)

## Binaries, Houdini 17.5
* [Windows, Houdini 17.5.360](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.4/houdini.geo.vox.17.5.360.win.rar)

## Binaries, Houdini 16
* [Windows, Houdini 16.0.557](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.3/houdini.geo.vox.16.0.557.win.rar)

## Binaries, Houdini 15
* [Mac OS X, Houdini 15.0.434](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.2/houdini.geo.vox.15.0.434.osx.tar.gz)
* [Windows, Houdini 15.0.440](https://github.com/ttvd/houdini-geo-vox/releases/download/1.0.2/houdini.geo.vox.15.0.440.win.rar)

## Building

* Tested on Windows and Houdini 17.5.
  * You would have to patch CMake file to get this building on Linux.
* Define HOUDINI_VERSION env variable to be the version of Houdini 17.5 you wish to build against (for example "17.5.360").
* Alternatively, you can have HFS env variable defined (set when you source houdini_setup).
* Generate build files from CMake for your favorite build system. For Windows builds use MSVC 2012.
* Build the GEO Houdini dso (GEO_Vox.dylib or GEO_Vox.dll).
* Place the dso in the appropriate Houdini dso folder.
  * On Windows this would be C:\Users\your_username\Documents\houdini17.5\dso

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
