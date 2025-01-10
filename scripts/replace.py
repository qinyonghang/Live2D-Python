import sys


def replace(file_path, pattern, replacement):
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

        content = content.replace(pattern, replacement)

        with open(file_path, "w", encoding="utf-8") as file:
            file.write(content)

        print(f"Successfully replaced content in {file_path}")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python replace.py <file_path> <pattern> <replacement>")
        print(sys.argv)
        sys.exit(1)

    file_path = sys.argv[1]
    pattern = sys.argv[2]
    replacement = sys.argv[3]
    print(f"Replacing content in {file_path}, pattern: {pattern}, replacement: {replacement}")
    replace(file_path, pattern, replacement)
