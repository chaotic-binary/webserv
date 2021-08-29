
import os

c = os.environ.get("HTTP_COOKIE")
if c == "name":
    print("Bulina")
else:
    print("hello world")