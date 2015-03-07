from distutils.core import setup, Extension

module1 = Extension('ppm',
                    sources = ['../lib/data.c', '../lib/node.c', '../lib/nodelist.c', '../lib/trie.c',
                               'ppm.c', 'trie.c', 'trie_members.c', 'trie_methods.c'],
                    include_dirs = ['../lib'])

setup (name = 'ppm',
       version = '1.0',
       description = 'This is a demo ppm package',
       ext_modules = [module1])
