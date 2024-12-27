import sys
import re


def replace_in_file(file_path):
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            content = file.read()

        pattern = r'MATCH "\(14\.3\)"'
        replacement = r'MATCH "(14.[34])"'  # 修正替换字符串
        new_content = re.sub(pattern, replacement, content)

        with open(file_path, "w", encoding="utf-8") as file:
            file.write(new_content)

        print(f"Successfully replaced content in {file_path}")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python replace.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    print(f"Replacing content in {file_path}")
    replace_in_file(file_path)
