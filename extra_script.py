import os
Import("env")

projectVersion = env.GetProjectOption("version")
envVersionOverride = os.getenv("VERSION", "")

# print(projectVersion)
# print(envVersionOverride)
if len(projectVersion) > 0:
    #print("set project version")
    env.Append(BUILD_FLAGS='-DRAW_VERSION=\"%s\"' % projectVersion)

if len(envVersionOverride) > 0:
    #print("set override version")
    env.Append(BUILD_FLAGS='-DRAW_VERSION=\"%s\"' % envVersionOverride)
