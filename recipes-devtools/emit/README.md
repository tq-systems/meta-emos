# emit

`emit` downloads app packages and builds signed Energy Manager RAUC bundles.
The `--help` option displays all commands and useful options.

## Bundle definitions

The bundle definition specifies important metadata and lists the apps installed in the bundle.
Bundle specifications can define shared app fields under `defaults` and override them for
individual apps under `apps`. The script merges defaults with the app definition, with app values
taking precedence. Nested mappings are merged recursively. When multiple bundle specifications are
provided, they are merged in order, so later specifications override earlier values.

### Compatible string

The compatible string must be unique for each product. It is used to distinguish products from one
another and ensure that a wrong firmware cannot be installed. If it is omitted, `emit` uses the
build's `--image-name`. The string is integrated into the rauc manifest along with the machine.

### Product information

Each bundle specification must contain a `product_info` mapping. `emit` writes its resolved values
to `/etc/product-info.json` in the image:

| Field | Mandatory | Default / derived value | Purpose |
| --- | --- | --- | --- |
| `manufacturer` | Yes | None | Manufacturer name; displayed in the user interface website. |
| `manufacturer_url` | Yes | None | Manufacturer website URL; displayed in the user interface website. |
| `name` | Yes | None | Product name; displayed in the user interface website. |
| `code` | Yes | None | Product code; used as the default hostname. |

### Apps definitions

Each entry under `apps` identifies an app by its mapping key. App fields can be set globally under
`defaults` or overridden for an individual app. The app definition supports these fields:

| Field | Mandatory | Default / derived value | Purpose |
| --- | --- | --- | --- |
| App ID | Yes | None | Identifies the app and supplies the default `{app}` URL path and `{name}` value. |
| `url` | Yes | None | Download URL template for the `.empkg` package. It is formatted with the merged app fields and the derived `{app}` and `{name}` values. |
| `version` | Conditional | None | App version, commonly used by the URL template. Required when the template references `{version}`. Versions `stable` and `latest` skip checksum handling. |
| `arch` | No | Value of emit's `--arch` option | Package architecture used by the URL template and to select `sha256[<arch>]`. Override it for architecture-independent packages such as `all`. |
| `name` | No | App ID | Overrides `{name}` in the URL template. |
| `variant` | No | No variant | Adds a variant subdirectory to `{app}` and derives `{name}` as `<app-id>-<variant>`. If `name` is also set, `name` takes precedence and the variant is not applied. |
| `cache` | No | `false` | Controls package caching and checksum behavior. When enabled, a missing checksum is downloaded from the package URL with `.sha256` appended. |
| `sha256[<arch>]` | No | Downloaded from the URL with `.sha256` appended when caching is enabled and a checksum is needed | Supplies the expected SHA-256 checksum for the selected architecture. |

An app with a `null` definition is skipped. Additional fields can also be used as placeholders in
`url` templates.

For example, shared URL and cache settings can be overridden for one app, while its version is
defined only for that app:

```yaml
defaults:
  url: https://packages.example.com/{app}/{name}_{version}_{arch}.empkg
  cache: true

apps:
  example-app:
    version: '1.2.3'
    cache: false
```

The URL is formatted using the merged app fields. If `cache` is not defined in either defaults
or the app definition, it defaults to `false`.
