# Publishing `esp32_p4_jc1060p470_board` to ESP Component Registry

This component is prepared to follow Espressif's publish tutorial:
https://docs.espressif.com/projects/idf-component-manager/en/latest/publish/tutorial_to_package_and_upload.html

## Prerequisites

- ESP-IDF environment activated
- `compote` installed (current environment reports `2.4.2`)
- GitHub account authenticated for registry upload

## Preflight checks

From this component directory:

```bash
cd components/esp32_p4_jc1060p470_board
compote component upload --name esp32_p4_jc1060p470_board --dry-run
```

Optional local build verification from the project root:

```bash
cd ../..
idf.py build
```

## Staging upload

1. Login to staging profile (see auth doc):
   https://docs.espressif.com/projects/idf-component-manager/en/latest/publish/how_to_authenticate.html#login-staging-registry

2. Upload current version to staging:

```bash
cd components/esp32_p4_jc1060p470_board
compote component upload --profile "staging" --name esp32_p4_jc1060p470_board
```

3. Consume staging build in a test project:

```yaml
dependencies:
  <your_namespace>/esp32_p4_jc1060p470_board:
    version: "*"
    registry_url: https://components-staging.espressif.com
```

## Production upload

1. Login to production (recommended browser flow):
   https://docs.espressif.com/projects/idf-component-manager/en/latest/publish/how_to_authenticate.html#login-via-cli

2. Upload current version:

```bash
cd components/esp32_p4_jc1060p470_board
compote component upload --name esp32_p4_jc1060p470_board
```

3. Optional namespace override:

```bash
compote component upload --name esp32_p4_jc1060p470_board --namespace <approved_namespace>
```

## Versioning rule

Each upload must use a new `version` in `idf_component.yml`.

Example release flow:

1. Bump `version` in `idf_component.yml`
2. Commit and tag in Git
3. Upload to staging
4. Validate from staging dependency
5. Upload to production
