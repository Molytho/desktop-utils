project(
	'swaybg-random',
	'cpp',
	version: '0.1.1',
	license: 'MIT',
	meson_version: '>=0.59.0',
	default_options: [
		'cpp_std=c++20',
		'warning_level=2',
		'werror=true',
	],
)

add_project_arguments(
	[
		'-Wno-unused-parameter',
		'-Wno-unused-result',
		'-Wundef',
		'-Wvla',
		'-Wno-error=write-strings'
	],
	language: 'cpp',
)

cpp = meson.get_compiler('cpp')

wayland_client = dependency('wayland-client')

executable(
	'swaybg-random',
	[
		'src/main.cpp',
		'src/wayland.cpp',
		'src/pictures.cpp',
		'src/output_handler.cpp',
		'src/output.cpp',
		'src/timer.cpp',
	],
	include_directories: 'include',
	dependencies: [
		wayland_client,
	],
	install: true
)
