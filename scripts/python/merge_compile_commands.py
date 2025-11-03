import json
from pathlib import Path

APP = Path("build/app/compile_commands.json")
OUT = Path("build/compile_commands.json")

if not APP.exists():
    print(f"❌ 没有找到 {APP}")
    exit(1)

with open(APP, "r", encoding="utf-8") as f:
    app_data = json.load(f)

merged = app_data

with open(OUT, "w", encoding="utf-8") as f:
    json.dump(merged, f, indent=2, ensure_ascii=False)

print(f"✅ 已生成合并后的 {OUT}")
