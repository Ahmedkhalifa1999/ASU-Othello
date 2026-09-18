import subprocess, time, os, signal, sys
SP=os.environ['SP']; os.environ['DISPLAY']=':99'
app=f"{SP}/build-warn/ASU-Othello"
def sh(c): return subprocess.run(c,shell=True,capture_output=True,text=True).stdout.strip()
def shot(name): sh(f"import -window root {SP}/gui/{name}.png"); print(f"[{time.time()-T0:6.1f}s] screenshot {name}")
def click(x,y): sh(f"xdotool mousemove {x} {y} click 1")
def board_hash(): return sh(f"import -window root -crop 800x800+161+0 +repage png:- | md5sum | cut -c1-8")
def scores(): return sh(f"import -window root -crop 1000x40+0+830 +repage png:- | md5sum | cut -c1-8")
sh("pkill -x ASU-Othello"); time.sleep(0.5)
log=open(f"{SP}/gui/app2.log","w")
p=subprocess.Popen([app],stdout=log,stderr=subprocess.STDOUT)
T0=time.time()
sh("timeout 15 xdotool search --sync --onlyvisible --class ASU-Othello"); time.sleep(1.5)
print("PID",p.pid)
# 1. black human plays (row2,col3)
click(511,250); time.sleep(1.0); shot("shot2_black_moved"); print("  board hash",board_hash())
# 2. white -> AI, then click the board to kick the AI
click(16,124); time.sleep(0.3)
t=time.time(); click(511,250)
# probe: is the GUI responsive during the AI's think+sleep? (xdotool getwindowname needs no app cooperation; use _NET_WM_PING via xprop is not simple) -> sample repaint instead
h0=board_hash(); time.sleep(0.5); h1=board_hash(); time.sleep(2.0); h2=board_hash()
print(f"  board hash right after kicking white AI: {h0}, +0.5s: {h1}, +2.5s: {h2}")
shot("shot3_white_ai_moved")
# 3. black -> AI as well; kick; AI vs AI now runs entirely inside one mouse event
click(984,124); time.sleep(0.3)
tk=time.time(); click(511,250)
hashes=[]
for i in range(40):
    time.sleep(1.0)
    alive = p.poll() is None
    hashes.append(board_hash())
    if i in (0,2,5,10,20,30): shot(f"shot4_ai_vs_ai_t{i+1}s")
    if not alive: break
print("  board hashes sampled each second during AI-vs-AI:", " ".join(hashes))
print("  distinct frames seen while game ran:", len(set(hashes)))
if p.poll() is None:
    print("  process still alive after 40s, waiting up to 120s more...")
    try: p.wait(120)
    except subprocess.TimeoutExpired: print("  still alive"); shot("shot5_final")
rc=p.poll()
print(f"[{time.time()-T0:6.1f}s] process exit status: {rc}", f"(killed by signal {-rc} = {signal.Signals(-rc).name})" if rc is not None and rc<0 else "")
time.sleep(0.5); shot("shot5_final")
log.close(); print("--- app2.log ---"); print(open(f"{SP}/gui/app2.log").read())
