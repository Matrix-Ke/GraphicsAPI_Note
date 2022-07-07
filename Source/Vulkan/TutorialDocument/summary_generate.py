import os
from pprint import pprint


def urlparse(url):
    return url.replace(' ', '%20')


def list_dir(path, res={"dir_name": '.', 'dir_children': [], 'files': []}):
    for i in os.listdir(path):
        temp_dir = os.path.join(path, i)
        if os.path.isdir(temp_dir):
            temp = {"dir_name": temp_dir, 'dir_children': [], 'files': []}
            res['dir_children'].append(list_dir(temp_dir, temp))
        else:
            if i != 'summary_generate.py' and i != 'SUMMARY.md':
                res['files'].append(i)
    res['dir_children'] = sorted(res['dir_children'], key=lambda item: item['dir_name'])
    res['files'] = sorted(res['files'])
    return res


def generate_summary(dir_tree, level=0):
    app = ''
    app += '#' * level + ' ' + dir_tree['dir_name'] + '\n\n'
    for file in dir_tree['files']:
        file_sp = file.split(' ')
        app += '- [{}]({}/{})\n'.format(file_sp[0] + ' ' + file_sp[1], urlparse(dir_tree['dir_name']), urlparse(file))
    app += '\n'
    for subdir in dir_tree['dir_children']:
        app += generate_summary(subdir, level+1) + '\n'
    return app


if __name__ == '__main__':
    dir_tree = list_dir('.')
    # pprint(dir_tree)
    app = generate_summary(dir_tree)
    print(app)
    pass
