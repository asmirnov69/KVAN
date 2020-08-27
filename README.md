# python

- have Anaconda3 python 3.8 installed in your system.
- to get access to KVAN python modules it is possible to use PYTHONPATH evn variable.
Define PYTHONPATH env as

```
export PYTHONPATH=<KVAN-git-checkout-dir>/python
```

- to verify KVAN code is operational:

```
cd <KVAN-git-checkout-dir>/python/tests
python test-fuargs.py test_fuargs[a=1,b=2,c=3]
```
