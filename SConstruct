import os

env = Environment()

env.Append(CXXFLAGS=['-std=c++11', '-Wall', '-g'])

env.Append(CPPPATH=['/usr/local/include'])



env.Append(LIBS=['pthread', 'boost_system', 'boost_regex', 'boost_thread', 'boost_filesystem', 'boost_program_options', 'glog'])


env.Append(LIBPATH=['/usr/local/lib'])


env.Program('p', ['goat.cc', 'gconnector.cc', 'packet.cc', 'ghandler.cc', 'app.cc'])

