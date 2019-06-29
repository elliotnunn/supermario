import argparse
import glob
import re
import os
from os import path


def splice(directory):
    OVERDIR = 'AmphibianDNA'
    try:
        overs = [n for n in os.listdir(path.join(directory, OVERDIR)) if path.splitext(n)[1].lower() not in ('.idump', '.rdump')]
    except FileNotFoundError:
        overs = []

    all_makefiles = list(glob.glob(path.join(glob.escape(directory), '**', '*.[Mm][Aa][Kk][Ee]'), recursive=True))

    overs_re = '|'.join(re.escape(x) for x in overs)
    overs_re = r'^[^#\s]*\b(Thing.lib)"?\s*ƒ\s*'.replace('Thing.lib', overs_re)
    overs_re = re.compile(overs_re, re.IGNORECASE)

    if overs:
        remaining = list(overs)

        # Method 1: replace an existing build rule
        for f in all_makefiles:
            mfile = open(f).read().split('\n')

            havechanged = False
            newmfile = []

            idx = -1
            while idx + 1 < len(mfile):
                idx += 1
                m = overs_re.match(mfile[idx])
                if m:
                    thefile = m.group(1)
                    havechanged = True
                    newmfile.append('# Rule replaced at build time by ' + path.basename(__file__))
                    remaining = [x for x in remaining if x.upper() != thefile.upper()]

                    srcfile = '{Sources}%s:%s' % (OVERDIR, thefile)
                    newmfile.append(m.group(0) + srcfile)
                    newmfile.append('\tDuplicate -y {Deps} {Targ}')

                    lastidx = idx # how many "old" lines should be commented out?
                    if mfile[idx].endswith('∂'):
                        while lastidx + 1 < len(mfile) and mfile[lastidx + 1].endswith('∂'):
                            lastidx += 1 # capture continuations of first line
                    while lastidx + 1 < len(mfile) and mfile[lastidx + 1].startswith('\t'):
                        lastidx += 1 # capture build lines starting with tab

                    while idx <= lastidx:
                        newmfile.append('#\t' + mfile[idx])
                        idx += 1
                else:
                    newmfile.append(mfile[idx])

            if havechanged:
                open(f, 'w').write('\n'.join(newmfile))

        if remaining:
            found_locations = {k: [] for k in remaining}

            overs_re = '|'.join(re.escape(x) for x in remaining)
            overs_re = r'^[^#]*"({\w+}(?:\w+:)*)(Thing.lib)"'.replace('Thing.lib', overs_re)
            overs_re = re.compile(overs_re, re.IGNORECASE)

            main_makefiles = []
            for f in all_makefiles:
                mfile = open(f).read().split('\n')

                if any(l.startswith('BuildDir') for l in mfile):
                    main_makefiles.append(f)

                for line in mfile:
                    m = overs_re.match(line)
                    if m:
                        orig_name = next(x for x in remaining if x.upper() == m.group(2).upper())
                        found_loc = m.group(1)+m.group(2)
                        if found_loc.upper() not in (x.upper() for x in found_locations[orig_name]):
                            found_locations[orig_name].append(found_loc)

            for f in main_makefiles:
                with open(f, 'a') as fd:
                    fd.write('\n# Rules created at build time by %s\n' % path.basename(__file__))
                    for orig_name, found_locs in found_locations.items():
                        if len(found_locs) == 1:
                            remaining = [x for x in remaining if x != orig_name]
                            fd.write(found_locs[0])
                            fd.write(' ƒ {Sources}%s:%s\n' % (OVERDIR, orig_name))
                            fd.write('\tDuplicate -y {Deps} {Targ}\n')

        diag = 'Successfully spliced: %d/%d' % (len(overs)-len(remaining), len(overs))
        if remaining: diag += '; Failed: ' + ' '.join(remaining)
        print(diag)
