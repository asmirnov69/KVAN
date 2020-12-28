import subprocess

def exe_cmd(cmd, raise_exception = True):
    e_code, out = subprocess.getstatusoutput(cmd)
    if raise_exception and e_code != 0:
        raise Exception(f"non-zero exit code: {cmd}")
    return out if e_code == 0 else None
    
def get_curr_tag():
    return exe_cmd("git describe --exact --tags --abbrev=0", False)

def get_curr_commit():
    return exe_cmd("git rev-parse HEAD", False)

def git_check_mods():
    cmd = "git diff-index --name-only --ignore-submodules HEAD --"
    out = exe_cmd(cmd)
    print(out)
    return len(out) > 0

def git_get_repo_top_dir():
    return exe_cmd("git rev-parse --show-toplevel").strip()

def extract_manifest(tarball_pn):
    cmd = f"tar tzf {tarball_pn} --wildcards --no-anchored MANIFEST.txt"
    ls = exe_cmd(cmd, raise_exception = True)
    if len(ls) != 1:
        raise Exception(f"tarball {tarball_pn} doesn't have unique MANIFEST.txt file")
    manifest_fn = ls[0]

    cmd = f"tar zxO {tarball_pn} {manifest_fn}"
    manifest_ls = exe_cmd(cmd, raise_exception = True)

    return manifest_fn, manifest_ls

def get_manifest_tag(manifest_pn):
    print("getting manifest tag from {manifest_pn}")
    with open(manifest_pn) as fd:
        tag = [s.strip() for s in fd.readlines() if s.find("tag:") == 0]
        if len(tag) == 0:
            raise Exception(f"can't find tag in manifest {manifest_fn}")
        elif len(tag) > 1:
            raise Exception(f"multiple tags found in manifest {manifest_fn}")
        tag = tag[0]
        print(f"tag from manifest: {tag}")

def verify_tarball(tarball_pn, manifest_tag):
    if manifest_tag == None:
        manifest_fn, manifest_ls = extract_manifest(tarball_pn)
        manifest_tag = get_manifest_tag(manifest_fn)
        print("manifest tag from {tarball_pn}: {manifest_tag}")
        
    for l in exe_cmd("tar tzvf {tarball_pn}", raise_exception = True):
        if l.find(manifest_tag + "/") != 0:
            m = f"tarball {tarball_pn} verification failed, tag was {manifest_tag}: {l}"
            raise Exception(m)

def unpack_tarball(tarball_pn, dest_dir):
    cmd = "tar zxf {tarball_pn} -C {dest_dir}"
    exe_cmd(cmd, raise_exception = True)
    
    
