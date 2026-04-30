# How to integrate ClaudCode with DeepSeekV4

## Install of Claudcode on Ubuntu 24.04

### Prerequisites

To get Claude running on your system, here is what you need:
- An instance of Ubuntu 24.04 with a minimum of 8GB RAM and 2 CPU cores. This can be a minimal server or a GUI desktop instance. Any of these will do just fine.
- A sudo user is configured.
- A Claude Pro, Max, or Teams subscription via Claude.ai, or an Anthropic Console account with active API billing. If you're on the free Claude.ai plan, you won't have access to Claude Code.
  Therefore, you need to head over to claude.ai or console.anthropic.com and upgrade before proceeding.

### Install Claude from the native installer 

```shell
$ curl -fsSL https://claude.ai/install.sh | bash
```
the output from terminal is like
```shell
✔ Claude Code successfully installed!

  Version: 2.1.63

  Location: ~/home/ericzang/.local/bin/claude

  Next: Run claude --help to get started

⚠ Setup notes:
  • Native installation exists but ~/.local/bin is not in your PATH. Run:

  echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc

✅ Installation complete!

```
Then run 

```shell
$ echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc
$ source ~/.bashrc
```

### Verify Claude

```shell
$ claude --version
```
Claude Code ships with a built-in diagnostic command that checks your setup end-to-end:
```shell
$ claude doctor
```


### Start Claude in your project

Claude Code's installation has been done now, but you still need to connect it to your Anthropic account.

Create your prejct folder, for example "my-first-claude-prj"

```shell
$ cd my-first-claude-prj
$ claude 
```
Claude Code will launch an initial setup walkthrough. First, you will be asked to select your preferred style of text.
Use the up/down arrow keys to navigate, then press ENTER to select your preferred style.

Next, select your Claude login method. If you have claude account with subscribtion(Pro, Max, Team and so on), you can select "claude account with subscribtion".
If you have an API billing with Anthropic, you also can log in with my Anthropic Console account.

Finally, Claude Code will launch a one-time OAuth authentication flow and open a browser window for you to log in with your Anthropic account.

To complete the setup, a quick safety check will be carried out, and you will be required to confirm whether you trust your workspace or the project folder from which you are running Claude code.

Once Claude is set up, it's recommended to initialize your workspace by running the command:

```
$ /init
```

Once you have initialized your workspace directory, you can start using Claude Code. 
For example, you can prompt it to create pythons code like an Interactive Math learning Activity for 7 to 8 years kid like personal tutor.


### The price of Athropic Claude Plan

[! Claude Plan Price](https://claude.com/pricing)


### The price of Athropic model token 
- Opus 4.7
- Sonnet 4.6
- Haiku 4.5

[!Claude models Price](https://claude.com/pricing#api)




## Token Efficiency DeepSeek V4
Opus 4.7 is brilliant for agentic code and DeepSeek V4 isn't smartest and isn't most powerful AGI model. But it is the most token efficeicny AGI model, 
[!DeepSeek V4 Token price](https://api-docs.deepseek.com/quick_start/pricing). 

I cannot afford the cost of claude token, so it is good idea to give DeepSeek V4 a shot.

Please refer to the deep seek documents 

[!Athropic APIs with Deep Seek](https://api-docs.deepseek.com/guides/anthropic_api)

[!claude code with Deep Seek](https://api-docs.deepseek.com/guides/agent_integrations/claude_code)

Basically, you need to edit a few environment variables before run claude in your project

```shell
export ANTHROPIC_BASE_URL=https://api.deepseek.com/anthropic
export ANTHROPIC_AUTH_TOKEN=<your DeepSeek API Key>
export ANTHROPIC_MODEL=deepseek-v4-pro[1m]
export ANTHROPIC_DEFAULT_OPUS_MODEL=deepseek-v4-pro[1m]
export ANTHROPIC_DEFAULT_SONNET_MODEL=deepseek-v4-pro[1m]
export ANTHROPIC_DEFAULT_HAIKU_MODEL=deepseek-v4-flash
export CLAUDE_CODE_SUBAGENT_MODEL=deepseek-v4-flash
export CLAUDE_CODE_EFFORT_LEVEL=max
```










