import os
def list_dir(dir):
	all_dirs = []
	for root, dirs, files in os.walk('./', True):
		for name in dirs:
			cur_dir = os.path.join(root, name)
			if cur_dir.find(".git") < 0:
				all_dirs.append(cur_dir)
	return all_dirs

cc_flags = {'CCFLAGS' : ['-D_LINUX', '-D_DEBUG', '-g', '-O0',]}
inc_flags = {'CPPPATH' : ['../header']}
obj = Glob('*.cpp')
all_dirs = list_dir('./')
print(all_dirs)
for dir in all_dirs:
	obj += Glob(dir + '/*.cpp')
	obj += Glob(dir + '/*.cc')

#lib1 = File('async-redis-cli/unix-include/lib/libevent.a')
lib1 = File('third_party/lib/libprotobuf.a')
lib2 = File('third_party/lib/libgtest.a')
lib3 = File('third_party/lib/libgtest_main.a')
path= ['/bin', '/usr/bin', '/opt/rh/devtoolset-8/root/usr/bin']
env = Environment(ENV={'PATH':path})
env.MergeFlags(inc_flags)
env.MergeFlags(cc_flags)
env.Program('epoll_test', list(obj), LIBS=[lib1, lib2, lib3,'pthread'])
