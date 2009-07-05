#!/usr/bin/python
# -*- coding: utf-8 -*-

atoms_intern_h = open("atoms_intern.h", "w")
atoms_h = open("atoms.h", "w")
header = open("header.txt", "r")

atoms_h.writelines(header.readlines())

try:
    atoms_list_file = open("atoms_list.txt", "r")
    atoms_list = atoms_list_file.readlines()
    #atoms_intern_h.write("#include <xcb/xcb.h>\n")
    for line in atoms_list:
        atoms_intern_h.write("xcb_atom_t %s;\n" % line.strip())

    atoms_intern_h.write("static atom_list_item_t atom_list[] =\n"
            "{\n")
    for line in atoms_list:
        atoms_intern_h.write('    { "%s", &%s },\n'
                % (line.strip(), line.strip()))
    atoms_intern_h.write("};\n\n");

except IOError, e:
    # atoms_list.txt no founded
    print(e)

for i in range(len(atoms_list)):
    atoms_h.write("extern xcb_atom_t %s;\n"
            % atoms_list[i].strip())

atoms_h.write("#endif")

atoms_list_file.close()
atoms_intern_h.close()
atoms_h.close()
