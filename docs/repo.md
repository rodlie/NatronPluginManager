# Natron Plug-in Repository Standard v1.0

**DRAFT, NOT IMPLEMENTED IN CLIENT YET**


# manifest.xml

All repositories should implement ``manifest.xml``.

```
<?xml version="1.0" encoding="utf-8"?>
<repo>
    <version>1.0</version>
    <title>A repository</title>
    <url>https://repository.org</url>
    <logo>https://repository.org/logo.png</logo>
    <zip>https://repository.org/download.zip</zip>
    <checksum>3cf24664724862401fce453e9020c3cd6727665b939c5b0bb8bd55bb1a8286eb</checksum>
    <changelog>https://repository.org/changelog.md</changelog>
    <modified>2021-11-25 19:00</modified>
</repo>
```

**NOTE:** the filename ``manifest.xml`` is not an requirement. The end-user adds a new repository by providing a direct url to the XML file *(``https://repository.org/manifest.xml``)*. This url is also used to check for updates.

### ``version``

Natron Plug-in Repository Standard version compatibility.

### ``title``

Title/Name of the repository.

### ``url``

Url to the homepage of the repository. This is optional.

### ``logo``

Direct url to a PNG image for the repository, should be at least 128x128 px. The image is downloaded once. This is optional.

### ``zip``

Direct url to a ZIP file containing the plug-ins.

### ``checksum``

``SHA-256`` checksum of the ZIP file. This is optional.

### ``changelog``

Direct url to a text file in markdown listing changes done in the repository. No external assets are allowed. This is optional.

### ``modified``

Last modified repository date. Bump this when something changes in the repository. Must be formated as `yyyy-mm-dd hh:mm`.


# PyPlug structure

Each plug-in must have it's own folder, including a minimum of one valid PyPlug .``py`` file, the filename must match the parent folder.

```
└── MyPlugin
    └── MyPlugin.py
```

The folder may contain any amount of files.

If the folder contains a ``README.md`` file it will be used instead of the description found in the ``.py`` file. External assets contained in the folder is allowed.

If the folder contains a ``ChangeLog.md`` file it will be used to display specific changes for the plug-in. External assets contained in the folder is allowed.
