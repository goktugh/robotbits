import os
import time


output_dir = None
output_f = None

def recorder_init():
    global output_dir,output_f
    output_dir = os.path.join(os.environ['HOME'], 'datarecorder') 
    os.makedirs(output_dir, exist_ok=True)
    # get the last id 
    last_id_fn = os.path.join(output_dir, 'last.id')
    try:
        with open(last_id_fn, 'rt') as f:
            last_id = int(f.readline().strip())
    except FileNotFoundError:
        last_id = 0
    id = last_id + 1
    with open(last_id_fn, 'wt') as f:
        print(id, file=f)

    output_fn = os.path.join(output_dir, '{:06}.txt'.format(id))
    output_f = open(output_fn, 'wt')

_fields = None

def format_field(v):
    if isinstance(v, float):
        return '{:.3f}'.format(v)
    return str(v)

def recorder_write(d):
    global _fields
    # Dict supplied
    if _fields is None:
        _fields = ['t'] + list(sorted(d.keys())) 
        # write header line
        print("\t".join(_fields), file=output_f)
    values = [time.clock_gettime(time.CLOCK_MONOTONIC)]
    for k in _fields[1:]:
        values.append(d.get(k))
    line = '\t'.join(map(format_field, values))
    print(line, file=output_f)
    output_f.flush()



