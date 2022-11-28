# Natron Plug-in Repository Standard v1.0

# Manifest

All repositories must implement a manifest XML file.

```
<?xml version="1.0" encoding="utf-8"?>
<repo>
    <version>1.0</version>
    <title>A repository</title>
    <url>https://repository.org</url>
    <manifest>https://repository.org/manifest.xml</manifest>
    <logo>https://repository.org/logo.png</logo>
    <zip>https://repository.org/download.zip</zip>
    <checksum>3cf24664724862401fce453e9020c3cd6727665b939c5b0bb8bd55bb1a8286eb</checksum>
    <modified>2021-11-25 19:00</modified>
</repo>
```

### ``version``

Natron Plug-in Repository Standard version compatibility.

### ``title``

Title/Name of the repository.

### ``url``

Url to the homepage of the repository. Must be ``http://`` or ``https://``. This is optional.

### ``manifest``

Direct url to the manifest XML file. Must be ``http://`` or ``https://``.

### ``logo``

Direct url to a PNG image for the repository, should be at least 128x128 px. The image is downloaded once (unless the url changes). Must be ``http://`` or ``https://``. This is optional.

### ``zip``

Direct url to a ZIP file containing the plug-ins. Must be ``http://`` or ``https://``.

### ``checksum``

``SHA-256`` checksum of the ZIP file. This is optional.

### ``modified``

Last modified repository date. Bump this when something changes in the repository. Must be formated as `yyyy-mm-dd hh:mm`. This is optional.

# PyPlug

Each PyPlug plug-in must have it's own folder, including a minimum of one valid PyPlug .``py`` file, the filename must match the parent folder.

```
└── MyPlugin
    └── MyPlugin.py
```

The folder may contain any amount of files.

* If the folder contains a ``README.md`` file it will be used instead of the description found in the ``.py`` file. External assets contained in the folder is allowed.

* If the folder contains a ``CHANGES.md`` file it will be used to display specific changes for the plug-in. External assets contained in the folder is allowed.

* If the folder contains a ``AUTHORS.md`` file it will be used to display the author(s) of the plug-in. External assets are not allowed.

# Add-on (DRAFT)

Each add-on must have it's own folder, including a minimum of one valid ``py`` file, the filename must match the parent folder.

```
└── MyAddon
    └── MyAddon.py
```

The folder must also contain a ``README.md`` file with the following structure:

```
[//]: # (ID : com.myplugin)
[//]: # (LABEL : MyPlugin)
[//]: # (VERSION : 1.0)
[//]: # (GROUP : Transform)
[//]: # (KEY : Key_E)
[//]: # (MODIFIER : ControlModifier)

ANYTHING YOU WANT, MARKDOWN OR PLAIN TEXT.
```

``KEY`` and ``MODIFIER`` is optional (shortcut for the add-on).
