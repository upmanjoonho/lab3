kimport os
import subprocess

def execute_command(command):
    try:
        if '|' in command:
            commands = command.split('|')
            processes = []
            previous_process = None
            
            for cmd in commands:
                cmd = cmd.strip().split()
                if previous_process is None:
                    process = subprocess.Popen(cmd, stdout=subprocess.PIPE)
                else:
                    process = subprocess.Popen(cmd, stdin=previous_process.stdout, stdout=subprocess.PIPE)
                processes.append(process)
                previous_process = process

            output, error = processes[-1].communicate()
            if output:
                print(output.decode(), end="")
            if error:
                print(error.decode(), end="")
            return

        if '>' in command:
            cmd, output_file = command.split('>', 1)
            cmd = cmd.strip().split()
            output_file = output_file.strip()
            with open(output_file, 'w') as outfile:
                subprocess.run(cmd, stdout=outfile)
            return
        elif '<' in command:
            cmd, input_file = command.split('<', 1)
            cmd = cmd.strip().split()
            input_file = input_file.strip()
            with open(input_file, 'r') as infile:
                subprocess.run(cmd, stdin=infile)
            return

        subprocess.run(command.split())

    except Exception as e:
        print(f"Error: {e}")

def shell():
    print("Custom Shell - Type 'exit' to quit.")
    while True:
        try:
            command = input(">>> ")
            if command.strip().lower() == 'exit':
                break
            execute_command(command)
        except KeyboardInterrupt:
            print("\nKeyboardInterrupt. Use 'exit' to quit.")
        except EOFError:
            break

if __name__ == "__main__":
    shell()

