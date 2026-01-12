project(
    'keepassxc-secret-service-no-password',
    'cpp',
    version : '0.1',
    meson_version : '>= 1.3.0',
    default_options : ['warning_level=3', 'cpp_std=c++20']
)

dependencies = [
]

sources = [
    'keepassxc-secret-service-no-password.cpp',
]

exe = executable(
    'keepassxc-secret-service-no-password',
    sources,
    install : true,
    dependencies : dependencies
)

test('basic', exe)
