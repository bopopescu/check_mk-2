#!/usr/bin/env python
# encoding: utf-8

import os
import stat

def test_init_scripts(site):
    scripts = [
        "apache",
        "core",
        "crontab",
        "icinga",
        "mkeventd",
        "nagios",
        "npcd",
        "nsca",
        "pnp_gearman_worker",
        "rrdcached",
        "xinetd",
    ]

    if site.version.edition() == "enterprise":
        scripts += [
            "cmc",
            "liveproxyd",
            "mknotifyd",
        ]

    installed_scripts = os.listdir(os.path.join(site.root, "etc/init.d"))

    assert sorted(scripts) == sorted(installed_scripts)
