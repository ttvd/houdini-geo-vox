import hou, sys
root = hou.node("/obj")
obj = root.createNode("geo")
file = obj.createNode("file")
# file.parm("file").set("/home/symek/work/houdini-geo-vox/example/cube.vox")
file.parm("file").set("/home/symek/work/voxel-model/vox/monument/monu7.vox")
points = obj.createNode("pointsfromvolume")
points.setFirstInput(file)
points.parm("particlesep").set(1)
geometry = file.geometry()
geometry.saveToFile("/home/symek/work/houdini-geo-vox/example/cube.bgeo")
sys.exit()

