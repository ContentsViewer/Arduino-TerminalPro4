using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEngine.UI;

public class MakeMusicChart : MonoBehaviour
{
    public string fileName = "Chart.txt";
    public GameObject noteObj;
    public Scrollbar scrollbar;
    public InputField fileNameInputField;
    public float interval = 15.0f;
    public float noteIntervalX = 10.0f;
    public float noteIntervalY = 10.0f;
    public float acceptableTime = 0.1f;
    public Color noteHitColor = Color.red;
    public Color noteDefaultColor = Color.green;
    //public Color noteRmvColor = Color.yellow;
    public float mouseWheelSensitivity = 10.0f;
    public CameraController.Param cameraParamInline;
    public CameraController.Param cameraParamOverview;
    public CameraController.Param cameraParamInlineRev;
    public CameraController.Param cameraParamOverviewRev;

    //public Transform cameraPosTrfm;

    public Transform playbackPositionTrfm;

    AudioSource audioSource;
    float audioTimePrev = 0.0f;
    bool cameraSw = false;
    bool cameraRevSw = false;
    CameraController cameraCtrl;

    class Note
    {
        public GameObject go;
        public enum TYPE
        {
            LU,
            LD,
            RU,
            RD,
            LL,
            RR,
            LR,
            RL
        }
        public TYPE type;
        public float time;

        public Note(float time, TYPE type)
        {
            this.time = time;
            this.type = type;
        }

    }

    LinkedList<Note> noteList = new LinkedList<Note>();
    LinkedListNode<Note> scanLineBefore, scanLineAfter;
    LinkedListNode<Note> rmvStartLineBefore, rmvStartLineAfter;
    Dictionary<Note, bool> hitNoteList = new Dictionary<Note, bool>();

    bool isRemoving = false;

    void Awake()
    {
        audioSource = GetComponent<AudioSource>();
        fileNameInputField.text = fileName;
        cameraCtrl = Camera.main.GetComponent<CameraController>();

        SetCamera();
    }

    void Start()
    {

    }

    void Update()
    {

        //Debug.Log(scanLineBefore + "//" + scanLineAfter);


        //Remove
        if (Input.GetKey(KeyCode.R))
        {
            isRemoving = true;
        }
        else
        {
            isRemoving = false;
        }

        if (Input.GetKeyDown(KeyCode.B))
        {
            cameraSw ^= true;
            SetCamera();

        }

        if (Input.GetKeyDown(KeyCode.V))
        {
            
            cameraRevSw ^= true;
            SetCamera();
        }
        //Save
        if (Input.GetKeyDown(KeyCode.Return))
        {
            Debug.Log("Save");
            SaveFile();
        }


        scrollbar.value = audioSource.time / audioSource.clip.length;

        //再生中
        if (audioSource.isPlaying)
        {

            if (Input.GetKeyDown(KeyCode.Space))
            {
                audioSource.Pause();
            }


            //LU
            if (Input.GetKeyDown(KeyCode.W))
            {
                //Debug.Log("LU" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.LU));
            }

            //LR;LL
            if (Input.GetKeyDown(KeyCode.S))
            {
                //Debug.Log("LR" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.LR));
            }

            //LD
            if (Input.GetKeyDown(KeyCode.Z))
            {
                //Debug.Log("LD" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.LD));
            }

            //RU
            if (Input.GetKeyDown(KeyCode.I))
            {
                //Debug.Log("RU" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.RU));
            }

            //RL;RR
            if (Input.GetKeyDown(KeyCode.J))
            {
                //Debug.Log("RL" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.RL));
            }

            //RD
            if (Input.GetKeyDown(KeyCode.M))
            {
                //Debug.Log("RD" + audioSource.time);
                AddNote(new Note(audioSource.time, Note.TYPE.RD));
            }
        }
        else
        {
            if (Input.GetKeyDown(KeyCode.Space))
            {
                audioSource.Play();
            }

            //MouseWheel
            float scroll = Input.GetAxis("Mouse ScrollWheel");
            float nextTime = audioSource.time + scroll * mouseWheelSensitivity;
            audioSource.time = Mathf.Clamp(nextTime, 0.0f, audioSource.clip.length);


        }

        //再生位置更新
        playbackPositionTrfm.position = new Vector3(0.0f, 0.0f, audioSource.time * interval);

        //NoteScanLine更新; 色更新
        //時間が進む向き
        if (audioSource.time - audioTimePrev > 0.0f)
        {
            //ScanLineAfter,Before更新
            for (;;)
            {
                if ((scanLineAfter != null) && (scanLineAfter.Value.time < audioSource.time))
                {
                    if (isRemoving)
                    {
                        var rmved = scanLineAfter;
                        scanLineAfter = scanLineAfter.Next;
                        RemoveNote(rmved);
                    }
                    else
                    {

                        scanLineBefore = scanLineAfter;
                        scanLineAfter = scanLineAfter.Next;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        //時間と逆向き
        else if (audioSource.time - audioTimePrev < 0.0f)
        {
            //ScanLineAfter,Before更新
            for (;;)
            {
                if ((scanLineBefore != null) && (scanLineBefore.Value.time > audioSource.time))
                {
                    if (isRemoving)
                    {
                        var rmved = scanLineBefore;
                        scanLineBefore = scanLineBefore.Previous;
                        RemoveNote(rmved);
                    }
                    else
                    {
                        scanLineAfter = scanLineBefore;
                        scanLineBefore = scanLineBefore.Previous;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        {
            LinkedListNode<Note> node = scanLineBefore;
            while (node != null)
            {
                if (!hitNoteList.ContainsKey(node.Value) && node.Value.time > audioSource.time - acceptableTime)
                {
                    node.Value.go.GetComponent<MeshRenderer>().material.color = noteHitColor;
                    hitNoteList.Add(node.Value, true);
                }
                else
                {
                    break;
                }

                node = node.Previous;
            }

            node = scanLineAfter;
            while (node != null)
            {
                if (!hitNoteList.ContainsKey(node.Value) && node.Value.time < audioSource.time + acceptableTime)
                {
                    node.Value.go.GetComponent<MeshRenderer>().material.color = noteHitColor;
                    hitNoteList.Add(node.Value, true);

                }
                else
                {
                    break;
                }

                node = node.Next;
            }

            List<Note> removeTargetList = new List<Note>();
            foreach (var keyVal in hitNoteList)
            {
                if (Mathf.Abs(keyVal.Key.time - audioSource.time) > acceptableTime)
                {
                    removeTargetList.Add(keyVal.Key);
                }
            }
            foreach (var key in removeTargetList)
            {
                if (key.go != null)
                {
                    key.go.GetComponent<MeshRenderer>().material.color = noteDefaultColor;
                }
                hitNoteList.Remove(key);
            }
        }
        //Debug.Log(noteScanLineBegin);
        //Debug.Log(noteScanLineEnd);
        audioTimePrev = audioSource.time;
    }
    /*
    public void LoadFile()
    {
        if(fileName == string.Empty)
        {
            HUD_MessageManager.instance.ShowAlert("<color=#FFFFFF{a}>Please enter fileName...</color>");
            return;
        }
        if (!File.Exists(fileName))
        {
            HUD_MessageManager.instance.ShowAlert("<color=#FFFFFF{a}>NotExist...</color>");
            return;
        }

        HUD_MessageManager.instance.ShowAlert("<color=#FFFFFF{a}>Loading...</color>");
        using (StreamReader reader = new StreamReader(fileName, Encoding.UTF8))
        {
            string text = "";

            //Time
            text += "{\r\n";
            foreach (var note in noteList)
            {
                text += "\t" + (ulong)(note.time * 1000) + ",\r\n";
            }
            text += "}\r\n";


            //Type
            text += "{\r\n";
            foreach (var note in noteList)
            {
                text += "\t" + (int)(note.type) + ",\r\n";
            }
            text += "}\r\n";
            writer.Write(text);

            writer.Close();
        }
    }
    */
    public void SaveFile()
    {
        if (fileName == string.Empty)
        {
            HUD_MessageManager.instance.ShowAlert("<color=#FFFFFF{a}>Please enter fileName...</color>");
            return;
        }
        HUD_MessageManager.instance.ShowAlert("<color=#FFFFFF{a}>Saving...</color>");
        using (StreamWriter writer = new StreamWriter(fileName, false, Encoding.UTF8))
        {
            string text = "";

            //Time
            text += "{\r\n";
            foreach (var note in noteList)
            {
                text += "\t" + (ulong)(note.time * 1000) + ",\r\n";
            }
            text += "}\r\n";


            //Type
            text += "{\r\n";
            foreach (var note in noteList)
            {
                text += "\t" + (int)(note.type) + ",\r\n";
            }
            text += "}\r\n";
            writer.Write(text);

            writer.Close();
        }
    }
    void RemoveNote(LinkedListNode<Note> node)
    {
        Destroy(node.Value.go);
        noteList.Remove(node);
    }

    void AddNote(Note note)
    {

        //ロード
        GameObject noteObjInstance = Instantiate(noteObj) as GameObject;
        noteObjInstance.name = noteObj.name;
        noteObjInstance.transform.parent = gameObject.transform;
        Vector3 pos = new Vector3(0.0f, 0.0f, 0.0f);
        pos.z = note.time * interval;

        switch (note.type)
        {
            case Note.TYPE.LD:
                pos.x = noteIntervalX * 2;
                pos.y = -noteIntervalY;
                break;

            case Note.TYPE.LR:
            case Note.TYPE.LL:
                pos.x = noteIntervalX;
                pos.y = 0.0f;
                break;

            case Note.TYPE.LU:
                pos.x = noteIntervalX * 2;
                pos.y = noteIntervalY;
                break;

            case Note.TYPE.RD:
                pos.x = -noteIntervalX * 2;
                pos.y = -noteIntervalY;
                break;

            case Note.TYPE.RL:
            case Note.TYPE.RR:
                pos.x = -noteIntervalX;
                pos.y = 0.0f;
                break;

            case Note.TYPE.RU:
                pos.x = -noteIntervalX * 2;
                pos.y = noteIntervalY;
                break;
        }
        noteObjInstance.transform.position = pos;
        noteObjInstance.GetComponent<MeshRenderer>().material.color = noteDefaultColor;
        note.go = noteObjInstance;

        //List登録
        if (scanLineAfter != null)
        {
            noteList.AddBefore(scanLineAfter, note);
            scanLineBefore = scanLineAfter.Previous;
        }
        else if (scanLineBefore != null)
        {
            noteList.AddAfter(scanLineBefore, note);
            scanLineBefore = scanLineBefore.Next;
        }
        else
        {
            noteList.AddFirst(note);
            scanLineBefore = noteList.First;
        }

    }

    void SetCamera()
    {
        if (cameraSw)
        {
            if (cameraRevSw)
            {
                cameraCtrl.SetCamera(cameraParamInlineRev);
            }
            else
            {
                cameraCtrl.SetCamera(cameraParamInline);
            }
        }
        else
        {
            if (cameraRevSw)
            {
                cameraCtrl.SetCamera(cameraParamOverviewRev);
            }
            else
            {
                cameraCtrl.SetCamera(cameraParamOverview);
            }
        }
    }

    public void ScrollAudioPosition(float value)
    {
        if (!audioSource.isPlaying)
        {
            audioSource.time = audioSource.clip.length * value;
        }
    }

    public void UpdateFileName(string fileName)
    {
        this.fileName = fileName;
    }
}
