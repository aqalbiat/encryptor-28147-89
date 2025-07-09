import os

def are_equal(file1, file2):
    content1 = open(file1, "r").read()
    content2 = open(file2, "r").read()
    return (content1 == content2)

tests = []
for i in range(5):
    tests.append(("data" + str(i), "encrypt" + str(i), "decrypt" + str(i), str(2 ** (i + 1)), "simple"))
for i in range(5, 10):
    tests.append(("data" + str(i), "encrypt" + str(i), "decrypt" + str(i), str(2 ** (i - 4)), "gamma"))

cnt = 0
serial_n = 0
for open_data, processed, deprocessed, quantity, regime in tests:
    current_cmd = ".\\test_single.bat"
    current_cmd += (" " + open_data)
    current_cmd += (" " + processed)
    current_cmd += (" " + deprocessed)
    current_cmd += (" " + quantity)
    current_cmd += (" " + regime)
    os.system(current_cmd)
    serial_n += 1
    ok = are_equal(".\\storage\\" + open_data + ".txt", ".\\storage\\" + deprocessed + ".txt")
    print("Current test #" + str(serial_n), end=' ')
    if ok:
        print("OK")
        cnt += 1
    else:
        print("Wrong")    

print("Overall,", cnt, "tests out of", serial_n, "passed")