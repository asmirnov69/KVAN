import sys
from KVAN import fuargs
import uuid

@fuargs.action
def gen_uuid(method, namespace = None, name = None):
    """
method: one of uuid1, uuid3, uuid4, uuid5
namespace: (optional) one of NAMESPACE_DNS, NAMESPACE_URL, NAMESPACE_OID, NAMESPACE_X500
name: (optional) any string value
    """
    if not method in ['uuid1', 'uuid3', 'uuid4', 'uuid5']:
        raise Exception("method %s unknown" % method)
    
    if method in ['uuid3', 'uuid5']:
        namespaces = ["NAMESPACE_DNS", "NAMESPACE_URL", "NAMESPACE_OID", "NAMESPACE_X500"]
        if namespace is None or not namespace in namespaces:
            raise Exception("namespace should be specified for method %s as one of %s" % (method, ",".join(namespaces)))
                
        if name is None:
            raise Exception("name should be specified for method %s" % method)

    if method == 'uuid1':
        print("uuid1:", uuid.uuid1())
    elif method == 'uuid3':
        namespace = eval("uuid.%s" % namespace)
        print("uuid3:", uuid.uuid3(namespace, name))
    elif method == 'uuid4':
        print("uuid4:", uuid.uuid4())
    elif method == 'uuid5':
        namespace = eval("uuid.%s" % namespace)
        print("uuid5:", uuid.uuid5(namespace, name))
    else:
        raise Exception("unknown value of method")

    return True

if __name__ == "__main__":
    fuargs.exec_actions(sys.argv[1:])
