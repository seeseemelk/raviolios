#!/usr/bin/env python3

import subprocess
import sys

if __name__ == "__main__":
    virtualAddress = int(sys.argv[1], 16)
    pid = int(sys.argv[2])
    with open("/proc/" + str(pid) + "/maps") as f:
        for line in f:
            partsA = line.split(' ')
            partsB = partsA[0].split('-')

            virtualStart = int(partsB[0], 16)
            virtualEnd = int(partsB[1], 16)
            physStart = int(partsA[2], 16)
            file = partsA[len(partsA) - 1].strip()
            if virtualAddress >= virtualStart and virtualAddress < virtualEnd:
                offset = virtualAddress - virtualStart + physStart
                result = subprocess.run(["addr2line", "-e", file, "-C", "-f", hex(offset)], stdout=subprocess.PIPE)
                lines = result.stdout.decode('utf-8').split('\n')
                function = lines[0].strip()
                filename = lines[1].strip()
                if "iot_firmware/" in filename:
                    filename = filename.split("iot_firmware/")[1]
                    print(function + " [" + filename + "]")
                else:
                    print(function)
