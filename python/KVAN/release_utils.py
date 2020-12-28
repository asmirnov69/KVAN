import subprocess, os

verbose = True
def exe_cmd(cmd, raise_exception = True, split_output = True):
    if verbose:
        print(f"exe_cmd: {cmd}")
    e_code, out = subprocess.getstatusoutput(cmd)
    if raise_exception and e_code != 0:
        raise Exception(f"non-zero exit code: {cmd}, {out}")

    if e_code != 0:
        ret = None
    else:        
        ret = out.split("\n") if split_output else out
        ret = [] if len(ret) == 1 and len(ret[0]) == 0 else ret

    return ret
    
def get_curr_tag():
    return exe_cmd("git describe --exact --tags --abbrev=0",
                   raise_exception = False, split_output = False)

def get_curr_commit():
    return exe_cmd("git rev-parse HEAD",
                   raise_exception = False, split_output = False)

def git_check_mods():
    cmd = "git diff-index --name-only --ignore-submodules HEAD --"
    out = exe_cmd(cmd)
    print("out: ", out)
    return len(out) > 0

def git_get_repo_top_dir():
    return exe_cmd("git rev-parse --show-toplevel")[0].strip()

def extract_manifest(tarball_pn):
    cmd = f"tar tzf {tarball_pn} --wildcards --no-anchored MANIFEST.txt"
    ls = exe_cmd(cmd)
    if len(ls) != 1:
        raise Exception(f"tarball {tarball_pn} doesn't have unique MANIFEST.txt file, {ls}")
    manifest_fn = ls[0]

    cmd = f"tar zxfO {tarball_pn} {manifest_fn}"
    manifest_ls = exe_cmd(cmd)

    return manifest_fn, manifest_ls

def get_manifest_tag(manifest_ls):
    tag = [s.strip() for s in manifest_ls if s.find("tag:") == 0]
    if len(tag) == 0:
        raise Exception(f"can't find tag in manifest {manifest_fn}")
    elif len(tag) > 1:
        raise Exception(f"multiple tags found in manifest {manifest_fn}")
    tag = tag[0].replace("tag: ", "")
    return tag

def verify_tarball(tarball_pn, manifest_tag):
    if manifest_tag == None:
        manifest_fn, manifest_ls = extract_manifest(tarball_pn)
        manifest_tag = get_manifest_tag(manifest_ls)
        print(f"manifest tag from {tarball_pn}: {manifest_tag}")
        
    for l in exe_cmd(f"tar tzf {tarball_pn}"):
        if l.find(manifest_tag + "/") != 0:
            m = f"tarball {tarball_pn} verification failed, tag was {manifest_tag}: {l}"
            raise Exception(m)

def unpack_tarball(tarball_pn, dest_dir):
    if not os.path.exists(dest_dir):
        print(f"creating dest_dir {dest_dir}")
        os.makedirs(dest_dir)
        
    cmd = f"tar zxf {tarball_pn} -C {dest_dir}"
    exe_cmd(cmd)
    
    
