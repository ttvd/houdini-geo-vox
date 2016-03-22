# houdini-geo-vox

Houdini GEO node which lets you load Magical Voxel Vox files.

![houdini-geo-vox screenshot](http://i.imgur.com/stmKung.png)

## Building

* Tested on OS X 10.11 and Houdini 15.
  * You would have to patch CMake file to get this building on Windows or Linux.
* Define HOUDINI_VERSION env variable to be the version of Houdini 15 you wish to build against (for example "15.0.389").
* Alternatively, you can have HFS env vatiable defined.
* Generate build files from CMake.
* Build the GEO Houdini dso (GEO_Vox.dylib or GEO_Vox.dll).
* Place the dso in the appropriate Houdini dso folder.
  * On OS X this would be /Users/your_username/Library/Preferences/houdini/15.0/dso/

## Usage

* Place a file SOP, select a .vox file.
* This will create a Houdini RGBA volume.

## License

* Copyright Mykola Konyk, 2016
* Distributed under the [MS-RL License.](http://opensource.org/licenses/MS-RL)
* **To further explain the license:**
  * **You cannot re-license any files in this project.**
  * **That is, they must remain under the [MS-RL license.](http://opensource.org/licenses/MS-RL)**
  * **Any other files you add to this project can be under any license you want.**
  * **You cannot use any of this code in a GPL project.**
  * Otherwise you are free to do pretty much anything you want with this code.
