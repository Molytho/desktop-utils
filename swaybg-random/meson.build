project(
	'swaybg-random',
	'cpp',
	version: '0.1.2',
	license: 'GPLv3',
	meson_version: '>=0.59.0',
	default_options: [
		'cpp_std=c++20',
		'warning_level=3',
		'werror=false',
	],
)

cpp = meson.get_compiler('cpp')

wayland_client = dependency('wayland-client')

src = [
#	'src/main.cpp',
#	'src/wayland.cpp',
#	'src/output_handler.cpp',
#	'src/output.cpp',
#	'src/timer.cpp',
#	'src/child_handler.cpp',
    'src/new/main.cpp',
    'src/new/event_loop.cpp',
	'src/new/picture_manager.cpp',
    'src/new/wayland/display.cpp',
	'src/new/loop_source/timer.cpp',
	'src/new/loop_source/signal.cpp'
]

executable(
	'swaybg-random',
	src,
	include_directories: 'include',
	dependencies: [
		wayland_client,
	],
	install: true
)
