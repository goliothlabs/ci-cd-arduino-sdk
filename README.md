# Using GitHub Actions and Golioth to Automatically Deploy IoT Firmware

On this demo, we show how to use Github Actions to build your device firmware using PlatformIO with Arduino SDK and push the binary to Golioth Platform. The binary is uploaded as an artifact to Golioth using our `goliothctl` tool. By default the workflow runs when you push a new git tag to a branch and the artifact is created matching that tag version.

- [Article with more details](https://blog.golioth.io/using-github-actions-and-golioth-to-automatically-deploy-iot-firmware/).
- [Video with demo](https://www.youtube.com/watch?v=FZinwXH0O1M).

You can see more details on the CI/CD workflow for Github Actions, that is available on [release.yaml](./.github/release.yaml).

To work properly you have to set up some Github Actions variables/secrets:

- `GOLIOTH_PROJECT_ID`: Your project ID on Golioth.
- `GOLIOTH_API_KEY`: Generate a new API key within the same project informed as `GOLIOTH_PROJECT_ID`. This is used to authenticate to our platform and push the binaries.
