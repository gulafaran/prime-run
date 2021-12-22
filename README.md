# prime-run

tool to help run on nvidia dgpus in laptops and sanity check its vars/options.

## Usage

```bash
To run an application on the NVIDIA gpu.
Use: prime-run <application>

To run various options prime-run handles.
Use: prime-run [options]

 Options:
  -g , --opengl              run a opengl test to see if NVIDIA gets used prints card to stdout.
  -x , --xorg                print an template xorg.conf.d conf to stdout.
  -u , --udev                print an template udev rule to disable NVIDIA PCI devices to stdout.
  -c , --check               print various nvidia powermanagement/prime related services and module options.
  -e , --envvars             print PRIME associated environment vars.
  -p , --powermanagement     print nvidia powermanagement status.
  -s , --status              print nvidia status file.
  -h , --help                print this help and exit.

```

## Contributing
Pull requests are welcome. bug reports aswell :)