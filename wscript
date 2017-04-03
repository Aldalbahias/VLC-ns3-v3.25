# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('vlc', ['core'])
    module.source = [
        'model/vlc-net-device.cc',
	'model/vlc-rx-net-device.cc',
	'model/vlc-tx-net-device.cc',
	'model/vlc-mobility-model.cc',
	'model/vlc-error-model.cc',
	'model/vlc-snr.cc',
	'model/vlc-channel-model.cc',
	'model/vlc-propagation-loss-model.cc',
        'helper/vlc-channel-helper.cc',
        'helper/vlc-device-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('vlc')
    module_test.source = [
        'test/vlc-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'vlc'
    headers.source = [
        'model/vlc-net-device.h',
	'model/vlc-rx-net-device.h',
	'model/vlc-tx-net-device.h',
	'model/vlc-mobility-model.h',
	'model/vlc-error-model.h',
	'model/vlc-snr.h',
	'model/vlc-channel-model.h',
	'model/vlc-propagation-loss-model.h',
        'helper/vlc-channel-helper.h',
        'helper/vlc-device-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

