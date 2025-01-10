import argparse


def write(path: str, texts: list, encoding="utf-8"):
    with open(path, "w", encoding=encoding) as file:
        for text in texts:
            file.write(text + "\n")


def get_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("path", type=str)
    parser.add_argument("texts", nargs="+")
    parser.add_argument("--encoding", type=str, default="utf-8")
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    args = get_args()

    write(args.path, args.texts, args.encoding)
