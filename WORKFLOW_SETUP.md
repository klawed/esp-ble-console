# GitHub Workflow Setup

To enable automatic building and GitHub Pages deployment, create the following file:

`.github/workflows/build-sketch.yml`

```yaml
name: CI

on:
  push:
    branches: [ main, stable ]
  pull_request:
    branches: [ main, stable ]

permissions:
  contents: write

jobs:
  build:
    runs-on: ubuntu-latest

    steps:

    - uses: actions/checkout@v3
      with:
        fetch-depth: 0

    - uses: actions/cache@v3
      with:
        path: |
          ~/.cache/pip
          ~/.platformio/
          .pio
        key: ${{ runner.os }}-pio

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11' 

    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install platformio

    - name: Install platformIO packages
      run: pio pkg install

    - name: pio check
      run: pio check --fail-on-defect medium

    - name: Run PlatformIO
      run: platformio run

    - name: Merge Firmware
      run: |
        ./generate_merge_firmware_command.py esp32dev esp32-s3-devkitc-1 | tee merge_firmware.bash
        bash -ex ./merge_firmware.bash

    - name: Generate ESP Web Tools
      run: |
        ./generate_esp_web_tools.py
        find web/

    - name: Deploy to GitHub Pages
      if: ${{ github.event_name != 'pull_request' }}
      uses: JamesIves/github-pages-deploy-action@v4
      with:
        folder: web/
```

After creating this file:
1. Enable GitHub Pages in repository settings
2. Set source to "GitHub Actions"
3. Push changes to trigger the first build
4. Your firmware will be available at: https://klawed.github.io/esp-ble-console/
