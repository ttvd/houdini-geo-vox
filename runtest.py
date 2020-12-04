import hou, sys
root = hou.node("/obj")
obj = root.createNode("geo")
file = obj.createNode("file")

convert = True;

if convert:
    file.parm("file").set("/home/symek/work/houdini-geo-vox/example/cube.bgeo")
    file.cook()
    geometry = file.geometry()
    geometry.saveToFile("/home/symek/work/houdini-geo-vox/example/my_cube.vox")
    file.parm("file").set("/home/symek/work/houdini-geo-vox/example/my_cube.vox")
    file.cook()
else:
    file.parm("file").set("/home/symek/work/houdini-geo-vox/example/my_cube.vox")
    # file.parm("file").set("/home/symek/work/voxel-model/vox/monument/monu0.vox")
    # file.parm("file").set("/home/symek/work/houdini-geo-vox/example/cube.vox")
    file.cook()

sys.exit()

# points = obj.createNode("pointsfromvolume")
# points.setFirstInput(file)
# points.parm("particlesep").set(1)

