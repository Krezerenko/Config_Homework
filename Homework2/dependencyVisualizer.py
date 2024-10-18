import codecs
import xml.etree.ElementTree as ET
import subprocess
import requests
import json

def get_input(config_file):
    try:
        tree = ET.parse(config_file)
        root = tree.getroot()
        repo_path = root.find("repo_path").text
        output_file = root.find("output_file").text
        visualizer_path = root.find("visualizer_path").text
        return repo_path, output_file, visualizer_path

    except FileNotFoundError:
        print(f"Конфигурационный файл не найден: {config_file}")
    except ET.ParseError as e:
        print(f"Ошибка парсинга XML: {e}")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

def get_repo_info(repo_path):
    sp_path = repo_path.split("/")
    if sp_path[-1] == "":
        sp_path.pop()
    return sp_path[-2::]

def get_commits(owner, repo, branch="main"):
    url = f"https://api.github.com/repos/{owner}/{repo}/commits?sha={branch}"
    try:
        response = requests.get(url)
        response.raise_for_status()
        commits = response.json()
        return commits
    except requests.exceptions.RequestException as e:
        print(f"Ошибка при запросе по адресу {url}: {e}")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

def get_branches(owner, repo):
    url = f"https://api.github.com/repos/{owner}/{repo}/branches"
    try:
        response = requests.get(url)
        if response.status_code == 404:
            print(f"Репозиторий {repo} с владельцем {owner} не найден.")
            return []
        response.raise_for_status()
        branches = response.json()
        return branches
    except requests.exceptions.RequestException as e:
        print(f"Ошибка при запросе по адресу {url}: {e}")
    except Exception as e:
        print(f"Произошла ошибка: {e}")


def write_all_commits(owner, repo):
    global out, commits
    with open("commits.json", "w") as out:
        commits = []
        for branch in get_branches(owner, repo):
            commits += get_commits(owner, repo, branch["name"])
        json.dump(commits, out)

if __name__ == '__main__':

    repoPath, outputFile, visualizerPath = get_input("config.xml")
    ownerName, repoName = get_repo_info(repoPath)

    write_all_commits(ownerName, repoName)

    with open("commits.json", "r") as f:
        commits = json.load(f)
        mermaid_code = "graph TD\n"
        for commit in commits:
            message = commit["commit"]["message"].replace("\n", " ").replace("\"", "'")
            sha = commit["sha"]
            mermaid_code += f"C{sha}[\"{message}\"]\n"
            for parent in commit["parents"]:
                pSha = parent["sha"]
                mermaid_code += f"C{pSha} --> C{sha}\n"
        with codecs.open("out.mmd", "w", "utf-8") as out:
            out.write(mermaid_code)
        subprocess.check_output(f"wsl /home/krezon/node_modules/.bin/mmdc -i /mnt/e/Config_Homework/Homework2/{outputFile} -o /mnt/e/Config_Homework/Homework2/output.svg", shell=True).decode("utf-8")
        subprocess.run(f"{visualizerPath} file:/E:/Config_Homework/Homework2/output.svg", shell = True)

